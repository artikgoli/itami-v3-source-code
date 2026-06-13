#pragma once
class c_security {
public:
	void start_heartbeat( );
	void stop_heartbeat( );

	struct dll_info {
		HMODULE self;
		bool unload;
	} m_dllinfo;

	struct user_data_t
	{
		char m_username[32] = "user";
		char m_windows_username[32] = "";
		char m_sub_expiry[60] = "lifetime";
		uint8_t* m_avatar = NULL;
		char m_signature[37] = "fuck";
	} m_user;

private:
	std::atomic<bool> running = false;
	std::thread heartbeat_thread;

	void heartbeat_internal( );
};

inline c_security* g_security = new c_security();