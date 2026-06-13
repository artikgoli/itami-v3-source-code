#include "window_handler.h"
#include "renderer.h"
#include <cassert>
#include "..\utilities\daisy.h"
#include "..\features\menu\menu.h"
#include "..\features\aimbot\aimbot.h"
#include "..\features\esp\esp.h"

#include <dwmapi.h>
#include "../features/notifications/notifications.h"
#include "../features/damage/damage.h"
#include "limiter.h"
#include "../features/misc/nade helper/nade_helper.h"
#include "../features/misc/misc.h"
#include "fonts.h"
#pragma comment(lib, "dwmapi.lib")
//#include <tlhelp32.h>
//#include "../utilities/memory/xor.h"

static std::unique_ptr<frame_limiter> g_frame_limiter;
static bool g_limiter_initialized = false;

static bool in_focus( )
{
	HWND foreground = GetForegroundWindow( );
	if (!foreground)
		return false;

	DWORD pid = 0;
	GetWindowThreadProcessId( foreground, &pid );

	static HWND hwnd = FindWindowA( nullptr, "Counter-Strike 2" );
	if (!hwnd)
		return false;

	static DWORD game_pid = 0;
	if ( !game_pid )
		GetWindowThreadProcessId( hwnd, &game_pid );

	return pid == game_pid;
}

static void restore( HWND overlay )
{
	HWND foreground = GetForegroundWindow( );
	if (foreground != overlay)
		return;

	HWND window = FindWindowA( nullptr, "Counter-Strike 2" );
	if (!window)
		return;

	DWORD foreground_thread = GetWindowThreadProcessId( foreground, nullptr );
	DWORD hwnd_thread = GetWindowThreadProcessId( window, nullptr );

	AttachThreadInput( foreground_thread, hwnd_thread, TRUE );

	SetForegroundWindow( window );
	SetActiveWindow( window );
	SetFocus( window );

	AttachThreadInput( foreground_thread, hwnd_thread, FALSE );
}

int refresh_rate( HWND hwnd )
{
	if (!hwnd)
		return 60;

	HMONITOR monitor = MonitorFromWindow( hwnd, MONITOR_DEFAULTTONEAREST );
	if (!monitor)
		return 60;

	MONITORINFOEX mi {};
	mi.cbSize = sizeof( mi );

	if (!GetMonitorInfo( monitor, &mi ))
		return 60;

	DEVMODE dm {};
	dm.dmSize = sizeof( dm );

	if (!EnumDisplaySettings( mi.szDevice, ENUM_CURRENT_SETTINGS, &dm ))
		return 60;

	if (dm.dmDisplayFrequency < 30)
		return 60;

	return ( int ) dm.dmDisplayFrequency;
}

void c_window_handler::paint( )
{
	if ( !dx_handler->device )
		return;

	if ( g_frame_limiter )
		g_frame_limiter->limit( );

	if ( dx_handler->needs_resize )
	{
		dx_handler->resize_buffers( dx_handler->pending_w, dx_handler->pending_h );
		dx_handler->needs_resize = false;
	}

	const bool lock_in_nigga = in_focus( );
	const bool let_me_in = lock_in_nigga || ( menu && menu->is_open );

	float clear_color[ 4 ] = { 0.f, 0.f, 0.f, 0.f };
	dx_handler->context->OMSetRenderTargets( 1, &dx_handler->render_target, nullptr );
	dx_handler->context->ClearRenderTargetView( dx_handler->render_target, clear_color );

	actual_size( window_size );
	render->update_view_matrix( );
	daisy_pre_draw( );
	{
		if ( let_me_in )
		{
			aimbot->on_render( );
			esp->render_players( );
			esp->render_entities( );
			notif->draw( );
			damage->on_render( );
			nade_helper->on_render( );
			misc->on_render( );
		}

		if ( menu )
		{
			if ( menu->is_open )
				menu->on_render( );

			static bool last_open = menu->is_open;
			if ( last_open != menu->is_open )
			{
				SetWindowLong( window_handle, GWL_EXSTYLE, menu->is_open ?
					( WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW ) :
					( WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW ) );

				//if ( menu->is_open )
				//{
				//	ClipCursor( nullptr );
				//	SetCursor( LoadCursor( nullptr, IDC_ARROW ) );
				//}

				last_open = menu->is_open;
			}
		}
	}
	daisy_post_draw( );
	input_handler->update( );

	dx_handler->swap_chain->Present( 0, 0 );
}

void c_window_handler::actual_size( vector2d& output )
{
	static vector2d cached = {};
	static DWORD last_check = 0;
	const DWORD now = GetTickCount( );
	if ( now - last_check >= 50 )
	{
		last_check = now;
		int w {}, h {};
		c_read_batch batch;
		batch.queue<int>( sdk->engine2_dll( ).base + cs2_dumper::offsets::engine2_dll::dwWindowWidth,  &w )
		     .queue<int>( sdk->engine2_dll( ).base + cs2_dumper::offsets::engine2_dll::dwWindowHeight, &h );
		memory->flush_batch( batch );
		if ( w > 0 && h > 0 )
		{
			cached.x = ( float ) w;
			cached.y = ( float ) h;
		}
	}
	output = cached;
}

void c_window_handler::daisy_pre_draw( )
{
	if ( !dx_handler || !dx_handler->device )
		return;

	D3D11_VIEWPORT vp = {};
	vp.Width = daisy::daisy_t::s_viewport_width;
	vp.Height = daisy::daisy_t::s_viewport_height;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	dx_handler->context->RSSetViewports( 1, &vp );

	daisy::daisy_prepare( );
}

void c_window_handler::daisy_post_draw( )
{
	draw_pool->flush_buffers( );
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param )
{
	switch ( msg )
	{
	case WM_SIZE:
		if ( dx_handler->device != nullptr && w_param != SIZE_MINIMIZED )
		{
			dx_handler->pending_w = LOWORD( l_param );
			dx_handler->pending_h = HIWORD( l_param );
			dx_handler->needs_resize = true;
		}
		return 0;

	case WM_KEYDOWN:
		return 0;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	case WM_HOTKEY:
	{
		if ( w_param == 0x0001 || w_param == 0x0002 )
		{
			if ( menu )
				menu->is_open = !menu->is_open;
		}
		else if ( w_param == 0x0003 )
		{
			PostMessageA( hwnd, WM_DESTROY, 0, 0 );
		}
		break;
	}
	}

	input_handler->on_wndproc( msg, w_param, l_param );

	return DefWindowProc( hwnd, msg, w_param, l_param );
}

//enum ZBID
//{
//	ZBID_DEFAULT    = 0,
//	ZBID_DESKTOP    = 1,
//	ZBID_UIACCESS   = 2,
//};

//typedef HWND( WINAPI* fn_CreateWindowInBand )( _In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle,
//	_In_ int nX, _In_ int nY, _In_ int nWidth, _In_ int nHeight,
//	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band );

//static BOOL check_ui_access( DWORD* pdw_err, DWORD* pf_ui_access )
//{
//	BOOL result = FALSE;
//	HANDLE h_token;

//	if ( OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &h_token ) )
//	{
//		DWORD dw_ret_len;
//		if ( GetTokenInformation( h_token, TokenUIAccess, pf_ui_access, sizeof( *pf_ui_access ), &dw_ret_len ) )
//			result = TRUE;
//		else
//			*pdw_err = GetLastError( );
//		CloseHandle( h_token );
//	}
//	else
//		*pdw_err = GetLastError( );

//	return result;
//}

//static DWORD duplicate_winlogon_token( DWORD dw_session_id, DWORD dw_desired_access, PHANDLE ph_token )
//{
//	DWORD dw_err = ERROR_NOT_FOUND;
//	PRIVILEGE_SET ps;
//	ps.PrivilegeCount = 1;
//	ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

//	if ( !LookupPrivilegeValue( NULL, SE_TCB_NAME, &ps.Privilege[ 0 ].Luid ) )
//		return GetLastError( );

//	HANDLE h_snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
//	if ( h_snap == INVALID_HANDLE_VALUE )
//		return GetLastError( );

//	PROCESSENTRY32 pe;
//	pe.dwSize = sizeof( pe );
//	BOOL b_found = FALSE;

//	for ( BOOL b_cont = Process32First( h_snap, &pe ); b_cont; b_cont = Process32Next( h_snap, &pe ) )
//	{
//		if ( strcmp( pe.szExeFile, X( "winlogon.exe" ) ) != 0 )
//			continue;

//		HANDLE h_proc = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID );
//		if ( !h_proc ) continue;

//		HANDLE h_tok;
//		if ( OpenProcessToken( h_proc, TOKEN_QUERY | TOKEN_DUPLICATE, &h_tok ) )
//		{
//			BOOL f_tcb;
//			DWORD dw_ret_len, sid;
//			if ( PrivilegeCheck( h_tok, &ps, &f_tcb ) && f_tcb &&
//				GetTokenInformation( h_tok, TokenSessionId, &sid, sizeof( sid ), &dw_ret_len ) &&
//				sid == dw_session_id )
//			{
//				b_found = TRUE;
//				if ( DuplicateTokenEx( h_tok, dw_desired_access, NULL, SecurityImpersonation, TokenImpersonation, ph_token ) )
//					dw_err = ERROR_SUCCESS;
//				else
//					dw_err = GetLastError( );
//			}
//			CloseHandle( h_tok );
//		}
//		CloseHandle( h_proc );
//		if ( b_found ) break;
//	}

//	CloseHandle( h_snap );
//	return dw_err;
//}

//static DWORD create_ui_access_token( PHANDLE ph_token )
//{
//	DWORD dw_err = ERROR_FUNCTION_FAILED;
//	HANDLE h_self;

//	if ( !OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY | TOKEN_DUPLICATE, &h_self ) )
//		return GetLastError( );

//	DWORD dw_session_id = 0, dw_ret_len;
//	if ( GetTokenInformation( h_self, TokenSessionId, &dw_session_id, sizeof( dw_session_id ), &dw_ret_len ) )
//	{
//		HANDLE h_system;
//		dw_err = duplicate_winlogon_token( dw_session_id, TOKEN_IMPERSONATE, &h_system );
//		if ( dw_err == ERROR_SUCCESS )
//		{
//			if ( SetThreadToken( NULL, h_system ) )
//			{
//				if ( DuplicateTokenEx( h_self, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT,
//					NULL, SecurityAnonymous, TokenPrimary, ph_token ) )
//				{
//					BOOL b_ui = TRUE;
//					if ( !SetTokenInformation( *ph_token, TokenUIAccess, &b_ui, sizeof( b_ui ) ) )
//					{
//						dw_err = GetLastError( );
//						CloseHandle( *ph_token );
//					}
//				}
//				else
//					dw_err = GetLastError( );
//				RevertToSelf( );
//			}
//			else
//				dw_err = GetLastError( );
//			CloseHandle( h_system );
//		}
//	}
//	else
//		dw_err = GetLastError( );

//	CloseHandle( h_self );
//	return dw_err;
//}

//static DWORD prepare_ui_access( )
//{
//	DWORD dw_err, f_ui_access = 0;

//	if ( !check_ui_access( &dw_err, &f_ui_access ) )
//		return dw_err;

//	if ( f_ui_access )
//		return ERROR_SUCCESS;

//	HANDLE h_token;
//	dw_err = create_ui_access_token( &h_token );
//	if ( dw_err != ERROR_SUCCESS )
//		return dw_err;

//	STARTUPINFOA si {};
//	PROCESS_INFORMATION pi {};
//	GetStartupInfoA( &si );

//	std::string cmd = std::string( GetCommandLineA( ) ) + X( " --uiaccess" );
//	if ( CreateProcessAsUserA( h_token, NULL, cmd.data( ), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
//	{
//		CloseHandle( pi.hProcess );
//		CloseHandle( pi.hThread );
//		ExitProcess( 0 );
//	}

//	dw_err = GetLastError( );
//	CloseHandle( h_token );
//	return dw_err;
//}

void c_window_handler::create_window( HINSTANCE instance, int w, int h )
{
	WNDCLASS wc = { 0 };
	wc.hInstance = instance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = wnd_class_name( );
	wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	RegisterClass( &wc );

	//ATOM wnd_atom = RegisterClass( &wc );
	//HMODULE h_user32 = GetModuleHandleA( X( "user32.dll" ) );
	//fn_CreateWindowInBand p_cwib = h_user32
	//	? reinterpret_cast<fn_CreateWindowInBand>( GetProcAddress( h_user32, X( "CreateWindowInBand" ) ) )
	//	: nullptr;
	//DWORD ui_err = prepare_ui_access( );
	//HWND hwnd;
	//if ( ui_err != ERROR_SUCCESS && p_cwib )
	//{
	//	hwnd = p_cwib(
	//		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
	//		wnd_atom, L"not itami overlay",
	//		WS_POPUP,
	//		0, 0, w, h,
	//		nullptr, nullptr, instance, nullptr, ZBID_UIACCESS );
	//}
	//else
	//{

	HWND hwnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		wc.lpszClassName,
		"not itami overlay",
		WS_POPUP,
		0, 0, w, h,
		0, 0, instance, 0 );

	//}

	SetLayeredWindowAttributes( hwnd, 0, 255, LWA_ALPHA );
	const MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea( hwnd, &margins );
	ShowWindow( hwnd, SW_SHOW );
	window_handle = hwnd;

	RegisterHotKey( hwnd, 0x0001, NULL, VK_INSERT );
	RegisterHotKey( hwnd, 0x0002, NULL, VK_F11 );
#ifdef _DEVELOPER
	RegisterHotKey( hwnd, 0x0003, NULL, VK_END );
#endif

	dx_handler->init_dx( hwnd, w, h );
	g_frame_limiter = std::make_unique<frame_limiter>( refresh_rate( hwnd ) );
	daisy::daisy_initialize( dx_handler->device, dx_handler->context, ( float ) w, ( float ) h );

	draw_pool->buffer = std::make_unique<c_single_buffer>( );
	draw_pool->buffer->queue.create( );

	// FONT INIT
	draw_pool->font_default.create( "Verdana", 13, NONANTIALIASED_QUALITY, 0x0 );
	draw_pool->font_debug.create( "Tahoma", 15, NONANTIALIASED_QUALITY, 0x0 );
	draw_pool->font_esp.create( "Tahoma", 11, ANTIALIASED_QUALITY, 0x0 );
	draw_pool->font_esp_s.create_from_memory( smallest_pixel, sizeof( smallest_pixel ), "Smallest Pixel-7", 10, NONANTIALIASED_QUALITY, 0x0 );
	draw_pool->font_esp_pixel.create_from_memory( smallest_pixel, sizeof( smallest_pixel ), "Smallest Pixel-7", 11, NONANTIALIASED_QUALITY, 0x0 );
	draw_pool->font_smooth.create( "Segoe UI", 16, ANTIALIASED_QUALITY,  0x0 );
	draw_pool->font_smooth_big.create( "Segoe UI", 18, ANTIALIASED_QUALITY, 0x0 );

	draw_pool->buffer->atlas.create( { 4096.f, 4096.f } );
	draw_pool->create_textures( );
}

void c_window_handler::destroy_window( HINSTANCE instance )
{
	daisy::daisy_shutdown( );

	if ( dx_handler ) dx_handler->destroy_dx( );

	if ( window_handle )
	{
		DestroyWindow( window_handle );
		window_handle = nullptr;
	}
	UnregisterClass( wnd_class_name( ), instance );
}

void c_dx11_handler::destroy_dx( )
{
	if ( render_target )
	{
		render_target->Release( );
		render_target = nullptr;
	}

	if ( swap_chain )
	{
		swap_chain->Release( );
		swap_chain = nullptr;
	}

	if ( context )
	{
		context->Release( );
		context = nullptr;
	}

	if ( device )
	{
		device->Release( );
		device = nullptr;
	}
}

void c_dx11_handler::init_dx( HWND hwnd, int w, int h )
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = w;
	sd.BufferDesc.Height = h;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL feature_level;

	while ( FAILED( D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		0, feature_levels, 2,
		D3D11_SDK_VERSION, &sd,
		&swap_chain, &device,
		&feature_level, &context ) ) )
	{
		Sleep( 50 );
	}

	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
	device->CreateRenderTargetView( back_buffer, nullptr, &render_target );
	back_buffer->Release( );
}

void c_dx11_handler::resize_buffers( int w, int h )
{
	draw_pool->pre_reset( );

	if ( render_target )
	{
		render_target->Release( );
		render_target = nullptr;
	}

	swap_chain->ResizeBuffers( 0, w, h, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );

	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
	device->CreateRenderTargetView( back_buffer, nullptr, &render_target );
	back_buffer->Release( );

	daisy::daisy_t::s_viewport_width = ( float ) w;
	daisy::daisy_t::s_viewport_height = ( float ) h;

	draw_pool->post_reset( );

	if ( menu )
		menu->on_window_update( );
}

void c_window_handler::sync_to_cs( HWND target_hwnd )
{
	if ( !target_hwnd || !IsWindow( target_hwnd ) )
		return;

	vector2d cs_size = {};
	actual_size( cs_size );
	if ( cs_size.x == 0.f && cs_size.y == 0.f )
		return;

	RECT cs_rect = {};
	GetClientRect( target_hwnd, &cs_rect );

	POINT cs_pos = { cs_rect.left, cs_rect.top };
	ClientToScreen( target_hwnd, &cs_pos );
	SetWindowPos( window_handle, 0, cs_pos.x, cs_pos.y, ( int ) cs_size.x, ( int ) cs_size.y, 0 );
}
