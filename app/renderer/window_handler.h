#pragma once
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")


class c_window_handler
{
public:
	constexpr LPCSTR wnd_class_name( )
	{
		return "not itami";
	}

	void create_window( HINSTANCE instance, int w, int h );
	void destroy_window( HINSTANCE instance );

	void sync_to_cs( HWND target_hwnd );

	HWND window_handle {};
	HWND target_handle {};

	void paint( );
	void actual_size( vector2d& output );

	vector2d window_size {};
	bool should_commit_a_goth_girl_in_2010 = false;

private:
	void daisy_pre_draw( );
	void daisy_post_draw( );
};

inline auto window_handler = std::make_unique<c_window_handler>( );

class c_dx11_handler
{
public:
	void init_dx( HWND hwnd, int w, int h );
	void destroy_dx( );
	void resize_buffers( int w, int h );

	bool needs_resize = false;
	int pending_w = 0, pending_h = 0;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swap_chain = nullptr;
	ID3D11RenderTargetView* render_target = nullptr;
};

inline auto dx_handler = std::make_unique<c_dx11_handler>( );
