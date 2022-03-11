// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(GSDK_Module, Log, All);

class GSDK_API FGSDKModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool WaitForPlayers() const;

	void UpdatePlayerCounts(TArray<FString>& PlayerIds) const;

	bool IsPlayerExpected(FString PlayerId) const;

	void SetGameThreadTime(FDateTime Now) const;

	FString GetSessionCookieJsonString() const;
	
	/* @hw
	 * GSDK doesn't like it when we request exit ourselves, instead we return false to the health check
	 * and GSDK will call request shutdown itself.
	 */
	void RequestShutdown() const;

	bool IsHealthy() const;
private:
	bool bNoGSDK = false;
	
	void QueueGameThreadTimeCheck();
};
