#include "include/gsdklibrary.h"
#include <stdexcept>
#include "gsdk.h"
#include <memory>

//static const std::vector<std::string> &getInitialPlayers();

/// <summary>
/// pass through for mainteance_callback that doesn't use windows specific types.
/// </summary>
static std::function<void(int, int, int, int, int, int, int, int, int)> maintenance_callback = nullptr;

/// <summary>
/// This should be cleaned up to provide the time in a portable manner to Unreal (no use of std library or windows librarires)
/// </summary>
/// <param name="time"></param>
void mainteance_callback_passthrough(const tm& time) {
	if (maintenance_callback != nullptr) {
		maintenance_callback(time.tm_sec, time.tm_min, time.tm_hour, time.tm_mday, time.tm_mon, time.tm_year, time.tm_wday, time.tm_yday, time.tm_isdst);
	}
}	

gsdk_library::gsdk_error_code gsdk_library::start(const bool debuglogs) {
	try {
		Microsoft::Azure::Gaming::GSDK::start(debuglogs);
	}
	catch (...) {
		//Assume that if we couldn't get a GSDK instance that we can't recover.
		return gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::ready_for_players(bool& is_ready) {
	try {
		is_ready = Microsoft::Azure::Gaming::GSDK::readyForPlayers();
	}
	catch (...) {
		//Assume that if we can't progress to the active state we can't recover.
		return gsdk_library::gsdk_error_code::UNRECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::update_connected_players(const char** const player_ids, const size_t player_id_count) {
	try {
		std::vector<Microsoft::Azure::Gaming::ConnectedPlayer> connected_players;
		for (unsigned int player_id_idx = 0; player_id_idx < player_id_count; ++player_id_idx) {
			connected_players.push_back(Microsoft::Azure::Gaming::ConnectedPlayer(player_ids[player_id_idx]));
		}
		Microsoft::Azure::Gaming::GSDK::updateConnectedPlayers(connected_players);
	}
	catch (...) {
		return gsdk_library::gsdk_error_code::RECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::register_shutdown_callback(void (callback)())
{
	try {
		Microsoft::Azure::Gaming::GSDK::registerShutdownCallback(callback);
	}
	catch (...) {
		return gsdk_library::gsdk_error_code::RECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::register_health_callback(bool (callback)())
{
	try {
		Microsoft::Azure::Gaming::GSDK::registerHealthCallback(callback);
	}
	catch (...) {
		return gsdk_library::gsdk_error_code::RECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::register_maintenance_callback(void(callback)(int tm_sec, int tm_min, int tm_hour, int tm_mday, int tm_mon, int tm_year, int tm_wday, int tm_yday, int tm_isdst))
{
	try {
		maintenance_callback = callback;

		Microsoft::Azure::Gaming::GSDK::registerMaintenanceCallback(mainteance_callback_passthrough);
	}
	catch (...) {
		return gsdk_library::gsdk_error_code::RECOVERABLE_ERROR;
	}

	return gsdk_library::gsdk_error_code::NO_ERROR;
}

gsdk_library::gsdk_error_code gsdk_library::is_player_expected(const char* player_id, bool& is_expected)
{
	is_expected = false;

	try {
		auto& initial_players = Microsoft::Azure::Gaming::GSDK::getInitialPlayers();

		for (int i = 0; i < initial_players.size(); ++i) {

			if (strncmp(player_id, initial_players[i].c_str(), 128) == 0) {
				is_expected = true;
				return gsdk_library::gsdk_error_code::NO_ERROR;
			}
		}
		return gsdk_library::gsdk_error_code::NO_ERROR;
	}
	catch (...) {
		return gsdk_library::gsdk_error_code::RECOVERABLE_ERROR;
	}
}

const char* gsdk_library::get_gsdk_log_directory()
{
	std::string gsdk_logs_dir = Microsoft::Azure::Gaming::GSDK::getLogsDirectory();

	char* gsdk_log_dir = new char[gsdk_logs_dir.length() + 1]();

	strncpy_s(gsdk_log_dir, gsdk_logs_dir.length() + 1, gsdk_logs_dir.c_str(), gsdk_logs_dir.length());

	return gsdk_log_dir;
}

const char* gsdk_library::get_gsdk_session_cookie()
{
	std::unordered_map<std::string, std::string> config_settings = Microsoft::Azure::Gaming::GSDK::getConfigSettings();

	std::string& gsdk_session_cookie = config_settings[Microsoft::Azure::Gaming::GSDK::SESSION_COOKIE_KEY];

	char* temp_cookie = new char[gsdk_session_cookie.length() + 1]();

	strncpy_s(temp_cookie, gsdk_session_cookie.length() + 1, gsdk_session_cookie.c_str(), gsdk_session_cookie.length());

	return temp_cookie;
}
