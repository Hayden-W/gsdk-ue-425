/// <summary>
/// provides portable (from and to Unreal) functionality for gsdk
/// </summary>
namespace gsdk_library {
    /// <summary>
    /// Error codes used to communicate with UE when an exception has occured and been handled.
    /// </summary>
    enum class gsdk_error_code {
        NO_ERROR = 0,               //No error
        RECOVERABLE_ERROR = 1,      //A recoverable error (The server doesn't have to shut down)
        UNRECOVERABLE_ERROR = 2,    //An unrecoverable error (The server should shut down)
    };
	/// <summary>
	/// start a multiplayer dedicated server.
	/// </summary>
	/// <param name="debuglogs">should debug logs be produced</param>
    gsdk_error_code start(const bool debuglogs = false);
    /// <summary>
    /// indicate that this server is ready to take players.
    /// </summary>
    /// <returns>whether or not the server is allocated.</returns>
    gsdk_error_code ready_for_players(bool& is_ready);
    /// <summary>
    /// update the number of players connected to this server.
    /// </summary>
    /// <param name="player_ids">the player tags of connected players</param>
    /// <param name="player_id_count">the number of provided player tags</param>
    gsdk_error_code update_connected_players(const char** const player_ids, const size_t player_id_count);
    /// <summary>
    /// registers a callback whenever the server is shutdown i.e. through game manager.
    /// </summary>
    /// <param name="callback">the function to call during shutdown.</param>
    gsdk_error_code register_shutdown_callback(void (callback)());
    /// <summary>
    /// register a function that will be called when the sdk wants an indication of the servers health. false for unhealthy, true for healthy.
    /// </summary>
    /// <param name="callback">the function to be called.</param>
    gsdk_error_code register_health_callback(bool (callback)());
    /// <summary>
    /// register a function that will be called when a request to put this server into maintenance mode is made.
    /// </summary>
    /// <param name="callback">the function to be called.</param>
    gsdk_error_code register_maintenance_callback(void (callback)(int tm_sec,     // seconds after the minute - [0, 60] including leap second
        int tm_min,                                                             // minutes after the hour - [0, 59]
        int tm_hour,                                                            // hours since midnight - [0, 23]
        int tm_mday,                                                            // day of the month - [1, 31]
        int tm_mon,                                                             // months since January - [0, 11]
        int tm_year,                                                            // years since 1900
        int tm_wday,                                                            // days since Sunday - [0, 6]
        int tm_yday,                                                            // days since January 1 - [0, 365]
        int tm_isdst));                                                         // daylight savings time flag));
    /// <summary>
    /// Checks if a player id appears in the initialPlayers list, it's a string compare so don't use too often.
    /// </summary>
    /// <param name="player_id"> Player ID to check for </param>
    /// <param name="is_expected"> if the above ID was found in the list of initial players </param>
    gsdk_error_code is_player_expected(const char* player_id, bool& is_expected);
    /// <summary>
    /// Gets the log directory of the GSDK (i.e. a folder that can be accessed from the PlayFab dashboard for easy viewing)
    /// </summary>
    /// <param name="log_dir"> The directory to store a log file </param>
    const char* get_gsdk_log_directory();

    /// <summary>
    /// Gets the session cookie string that can be passed in when requesting a server. Only available after readyForPlayers returns true!
    /// </summary>
    /// <param name="session_cookie"> The session cookie string </param>
    const char* get_gsdk_session_cookie();
}