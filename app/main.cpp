bool executed = false;

#include "renderer\window_handler.h"
#include "renderer\renderer.h"
#include "utilities/vmprotect/VMProtectSDK.h"

static void render_thread_stub( HINSTANCE instance )
{
	HWND cs = FindWindowA( 0, "Counter-Strike 2" );
	if ( !cs )
		exit( 0 );

	RECT tr;
	GetWindowRect( cs, &tr );

	window_handler->create_window( instance, tr.right - tr.left, tr.bottom - tr.top );
	window_handler->target_handle = cs;

	MSG msg = {};
	DWORD last_housekeeping = 0;
	while ( msg.message != WM_QUIT )
	{
		const DWORD ghetto_paste = GetTickCount( );
		if ( ghetto_paste - last_housekeeping >= 500 )
		{
			last_housekeeping = ghetto_paste;
			if ( !FindWindowA( 0, "Counter-Strike 2" ) || !IsWindow( window_handler->target_handle ) )
			{
				PostMessageA( window_handler->window_handle, WM_QUIT, 0, 0 );
				break;
			}
			window_handler->sync_to_cs( window_handler->target_handle );
		}

		if ( PeekMessageA( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessageA( &msg );
		}

		window_handler->paint( );
	}

	window_handler->should_commit_a_goth_girl_in_2010 = true;
	dx_handler->destroy_dx( );
	window_handler->destroy_window( instance );
}

#ifndef _DEVELOPER
void release_main(HMODULE hInstance) {
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
#endif

  
#ifdef _DEVELOPER
	console->attach("my ghetto debugger");
#endif

	entry::init();
	std::thread overlay( render_thread_stub, hInstance );
	overlay.detach( );

#ifdef _DEVELOPER
	while ( !( GetAsyncKeyState( VK_DELETE ) & 1 ) )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	}

	console->destroy();
	return 0;
#else
	while ( !window_handler->should_commit_a_goth_girl_in_2010 )
		Sleep( 1500 );

	if ( window_handler->window_handle )
		PostMessageA( window_handler->window_handle, WM_QUIT, 0, 0 );

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	FreeLibraryAndExitThread( hInstance, 0 );
#endif
}


#ifndef _DEVELOPER
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID param) {
	if (reason == DLL_PROCESS_ATTACH) {

		if (!param)
			return false;

		c_security::user_data_t* nigga = reinterpret_cast<c_security::user_data_t*>(param);
		strcpy(g_security->m_user.m_username, nigga->m_username);
		strcpy(g_security->m_user.m_windows_username, nigga->m_windows_username);
		strcpy(g_security->m_user.m_sub_expiry, nigga->m_sub_expiry);
		strcpy(g_security->m_user.m_signature, nigga->m_signature);

		g_security->m_dllinfo.self = module;

		_beginthreadex(nullptr, NULL, reinterpret_cast<unsigned(__cdecl*)(void*)>(release_main), module, NULL, nullptr);

	}

	return TRUE;
}
#endif