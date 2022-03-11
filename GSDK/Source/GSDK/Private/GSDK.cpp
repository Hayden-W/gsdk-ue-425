// Copyright Epic Games, Inc. All Rights Reserved.

#include "GSDK.h"
#include "Core.h"
#include "Async/Async.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#if UE_SERVER
	#include "gsdklibrary.h"
#endif

#define LOCTEXT_NAMESPACE "FGSDKModule"

DEFINE_LOG_CATEGORY(GSDK_Module);

static void GSDK_ShutdownCallback()
{
	UE_LOG(GSDK_Module, Display, TEXT("GSDK requested shutdown."));
	//this is the only place the server should shutdown
	FGenericPlatformMisc::RequestExit(true);
}

static void GSDK_MaintenanceCallback(int tm_sec,    // seconds after the minute - [0, 60] including leap second
    int tm_min,                                     // minutes after the hour - [0, 59]
    int tm_hour,                                    // hours since midnight - [0, 23]
    int tm_mday,                                    // day of the month - [1, 31]
    int tm_mon,                                     // months since January - [0, 11]
    int tm_year,                                    // years since 1900
    int tm_wday,                                    // days since Sunday - [0, 6]
    int tm_yday,                                    // days since January 1 - [0, 365]
    int tm_isdst)
{
    /* @hw
     * This should broadcast a message to clients that the server will be shutdown for maintenance at the given time.
     * We should then stop broadcasting this server for connection, wait for it to empty, and then shutdown.
     */
}

static bool bIsHealthy = true;
static bool bShouldTestGameThread = false; //Don't want to test game thread until game is running.
static FDateTime GameThreadLatestTime;

static bool GSDK_HealthCallback()
{
	//If the game thread hasn't updated in 5 seconds something has gone wrong!
	if(bShouldTestGameThread && FDateTime::Now() > (GameThreadLatestTime + FTimespan(0,0,5)))
	{
		UE_LOG(GSDK_Module, Error, TEXT("Game thread hasn't responded for 5 seconds, something has gone wrong!"));
		bIsHealthy = false;
	}
	else
	{
		//Potential to recover after a stall?
		bIsHealthy = true;
	}
	
    // Maybe this should check some game thread callback to assure that the game thread hasn't stalled.
	if(!bIsHealthy) UE_LOG(GSDK_Module, Display, TEXT("GSDK heartbeat unhealthy!"));
    return bIsHealthy;
}

void FGSDKModule::StartupModule()
{
	if(FParse::Param(FCommandLine::Get(), TEXT("noGSDK")) || FParse::Param(FCommandLine::Get(), TEXT("nogsdk")))
	{
		UE_LOG(GSDK_Module, Display, TEXT("Found noGSDK flag in launch args, no GSDK calls will be made."));
		bNoGSDK = true;
		return;
	}

#if UE_SERVER
	const char* LoggingDirectory = gsdk_library::get_gsdk_log_directory();

	//this isn't great lol, should really get the log dir stuff working correctly.
	FString ServerLogDirectory = FString(LoggingDirectory);
	FString ServerLogPath = FPaths::Combine(ServerLogDirectory, TEXT("ServerLog.log"));
	FOutputDeviceRedirector::Get()->AddOutputDevice(new FOutputDeviceFile(*ServerLogPath));
	
	UE_LOG(GSDK_Module, Display, TEXT("Log output file: %s"), *ServerLogPath);
	
	gsdk_library::gsdk_error_code StartResult = gsdk_library::start();
	if(StartResult == gsdk_library::gsdk_error_code::NO_ERROR)
	{
		UE_LOG(GSDK_Module, Display, TEXT("GSDK Started"));
	}
	else if(StartResult == gsdk_library::gsdk_error_code::RECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Warning, TEXT("GSDK start returned a recoverable error..."));
	}
	else if(StartResult == gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR)
	{
		//We've encountered an unrecoverable error, so we'll shutdown the server.
		UE_LOG(GSDK_Module, Error, TEXT("Failed to start GSDK instance, will attempt to shutdown."));
		RequestShutdown();
		return;
	}

	gsdk_library::gsdk_error_code HealthCallbackResult = gsdk_library::register_health_callback(GSDK_HealthCallback);
	if(HealthCallbackResult == gsdk_library::gsdk_error_code::NO_ERROR)
	{
		UE_LOG(GSDK_Module, Display, TEXT("Registered Health Callback"));
	}
	else if(HealthCallbackResult == gsdk_library::gsdk_error_code::RECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Warning, TEXT("Failed to register health callback."));
	}

	gsdk_library::gsdk_error_code ShutdownCallbackResult = gsdk_library::register_shutdown_callback(GSDK_ShutdownCallback);
	if(ShutdownCallbackResult == gsdk_library::gsdk_error_code::NO_ERROR)
	{
		UE_LOG(GSDK_Module, Display, TEXT("Registered Shutdown Callback"));
	}
	else if(ShutdownCallbackResult == gsdk_library::gsdk_error_code::RECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Warning, TEXT("Failed to register Shutdown callback."));
	}

	gsdk_library::gsdk_error_code MaintenenceCallbackResult = gsdk_library::register_maintenance_callback(GSDK_MaintenanceCallback);
	if(MaintenenceCallbackResult == gsdk_library::gsdk_error_code::NO_ERROR)
	{
		UE_LOG(GSDK_Module, Display, TEXT("Registered Maintenence Callback"));
	}
	else if(MaintenenceCallbackResult == gsdk_library::gsdk_error_code::RECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Warning, TEXT("Failed to register Maintenence callback."));
	}
#endif
}

void FGSDKModule::ShutdownModule()
{
	bShouldTestGameThread = false;
}

bool FGSDKModule::WaitForPlayers() const
{
	if(bNoGSDK) return true;

#if UE_SERVER
	bool bIsReady = false;
	gsdk_library::gsdk_error_code ErrorCode = gsdk_library::ready_for_players(bIsReady);
	if(ErrorCode == gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Error, TEXT("Ready for Players returned an unrecoverable error."));
		RequestShutdown();
		return false;
	}
	else if(ErrorCode == gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Warning, TEXT("Ready for Players returned a recoverable error."));
	}
	bShouldTestGameThread = true;
	GameThreadLatestTime = FDateTime::Now();
	
	FString TestSessionCookie = GetSessionCookie();
	
	UE_LOG(GSDK_Module, Display, TEXT("returned cookie: %s"), *TestSessionCookie);
	
	return bIsReady;
#else
    return false;
#endif

	
}

void FGSDKModule::UpdatePlayerCounts(TArray<FString>& PlayerIds) const
{
	if(bNoGSDK) return;
	
#if UE_SERVER
    const uint32 PlayerIdCount = PlayerIds.Num();
	TUniquePtr<const char*[]> PlayerIdsAsConstChar = MakeUnique<const char*[]>(static_cast<SIZE_T>(PlayerIdCount));

    TArray<TUniquePtr<const char[]>> AnsiPlayerIds;

    for (unsigned int IdIdx = 0; IdIdx < PlayerIdCount; ++IdIdx)
    {
        auto& IdCharArray = PlayerIds[IdIdx].GetCharArray();

        TUniquePtr<const char[]>& NewAnsiPlayerId = AnsiPlayerIds.Add_GetRef(MakeUnique<const char[]>(IdCharArray.Num()));

        FMemory::Memcpy(const_cast<char*>(NewAnsiPlayerId.Get()), TCHAR_TO_ANSI(IdCharArray.GetData()), IdCharArray.Num());

        PlayerIdsAsConstChar.Get()[IdIdx] = NewAnsiPlayerId.Get();
    }
	gsdk_library::gsdk_error_code ErrorCode = gsdk_library::update_connected_players(
		PlayerIdsAsConstChar.Get(),
		PlayerIdCount
	);

	if(ErrorCode == gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Error, TEXT("Update connected players returned an unrecoverable error."));
		RequestShutdown();
	}
	else if(ErrorCode == gsdk_library::gsdk_error_code::RECOVERABLE_ERROR)
	{
		UE_LOG(GSDK_Module, Error, TEXT("Update connected players returned a recoverable error."));
	}
#endif
}

bool FGSDKModule::IsPlayerExpected(FString PlayerId) const
{
	if(bNoGSDK) return false;
#if UE_SERVER
	bool bIsExpected = false;
	const char* CharPlayerId = TCHAR_TO_ANSI(*PlayerId);
	
	gsdk_library::is_player_expected(CharPlayerId, bIsExpected);
	
	return bIsExpected;
#else 
	return false;
#endif
}

void FGSDKModule::SetGameThreadTime(FDateTime Now) const
{
	GameThreadLatestTime = Now;
}

FString FGSDKModule::GetSessionCookieJsonString() const
{
#if UE_SERVER

	return FString(gsdk_library::get_gsdk_session_cookie());
	
#else 
	return "";
#endif
}

void FGSDKModule::RequestShutdown() const
{
	UE_LOG(GSDK_Module, Display, TEXT("Shutdown requested."));
	//Not a member variable so const is OK
	bIsHealthy = false;
	
	if(bNoGSDK)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
	else
	{
		FGenericPlatformMisc::RequestExit(true);
	}
}

bool FGSDKModule::IsHealthy() const
{
	return bIsHealthy;
}

void FGSDKModule::QueueGameThreadTimeCheck()
{
	AsyncTask(ENamedThreads::GameThread, []() -> void { GameThreadLatestTime = FDateTime::Now(); });
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGSDKModule, GSDK)
