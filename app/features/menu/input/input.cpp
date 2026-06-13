#include "input.h"
#include "..\menu.h"
#include "..\..\..\renderer\window_handler.h"
#include "..\..\aimbot\aimbot.h"
#include "..\..\misc\misc.h"

void c_input_handler::on_wndproc( UINT msg, WPARAM w_param, LPARAM l_param )
{
	// always call this first
	handle_key_menu_input( msg, w_param, l_param );

	int x = menu->position.x;
	int y = menu->position.y;

	static bool is_dragging = false;
	static vector2d drag_offset = { };
	static bool kbl_dragging = false;
	static vector2d kbl_drag_offset = { };
	static bool spec_dragging = false;
	static vector2d spec_drag_offset = { };
	static bool radar_dragging = false;
	static vector2d radar_drag_offset = { };
	static bool bomb_dragging = false;
	static vector2d bomb_drag_offset = { };

	// handle dragging
	int mouse_x = LOWORD( l_param );
	int mouse_y = HIWORD( l_param );

	vector2d mouse_pos = vector2d( mouse_x, mouse_y );
	menu->handle_tab_input( msg, mouse_pos );
	menu->handle_work_area_input( x + 10, y + 30 - 14, msg, mouse_pos );

	if ( menu->is_open )
		SetCapture( window_handler->window_handle );
	else
		ReleaseCapture( );

	int kbl_x    = ( int ) config_variables->misc.keybind_list_x->value;
	int kbl_y    = ( int ) config_variables->misc.keybind_list_y->value;
	int spec_x   = ( int ) config_variables->misc.spectator_list_x->value;
	int spec_y   = ( int ) config_variables->misc.spectator_list_y->value;
	int bomb_x   = ( int ) config_variables->misc.bomb_timer_x->value;
	int bomb_y   = ( int ) config_variables->misc.bomb_timer_y->value;
	int radar_x  = ( int ) config_variables->misc.radar_x->value;
	int radar_y  = ( int ) config_variables->misc.radar_y->value;
	int radar_rs = config_variables->misc.radar_size->value;

	switch ( msg )
	{
	case WM_LBUTTONDOWN:
	{
		if ( mouse_x >= x && mouse_x <= x + menu->size.x &&
			mouse_y >= y && mouse_y <= y + menu->size.y )
		{
			if ( mouse_y <= y + 18 )
			{
				is_dragging = true;
				drag_offset.x = mouse_x - x;
				drag_offset.y = mouse_y - y;
			}
		}

		if ( config_variables->misc.keybind_list->value &&
			mouse_x >= kbl_x && mouse_x <= kbl_x + misc->kbl_w &&
			mouse_y >= kbl_y && mouse_y <= kbl_y + 20 )
		{
			kbl_dragging = true;
			kbl_drag_offset.x = mouse_x - kbl_x;
			kbl_drag_offset.y = mouse_y - kbl_y;
		}

		if ( config_variables->misc.spectator_list->value &&
			mouse_x >= spec_x && mouse_x <= spec_x + 130 &&
			mouse_y >= spec_y && mouse_y <= spec_y + 20 )
		{
			spec_dragging = true;
			spec_drag_offset.x = mouse_x - spec_x;
			spec_drag_offset.y = mouse_y - spec_y;
		}

		if ( config_variables->misc.bomb_timer->value &&
			mouse_x >= bomb_x && mouse_x <= bomb_x + misc->bomb_w &&
			mouse_y >= bomb_y && mouse_y <= bomb_y + 20 )
		{
			bomb_dragging = true;
			bomb_drag_offset.x = mouse_x - bomb_x;
			bomb_drag_offset.y = mouse_y - bomb_y;
		}

		if ( config_variables->misc.radar_enabled->value &&
			mouse_x >= radar_x && mouse_x <= radar_x + radar_rs + 4 &&
			mouse_y >= radar_y && mouse_y <= radar_y + 20 )
		{
			radar_dragging = true;
			radar_drag_offset.x = mouse_x - radar_x;
			radar_drag_offset.y = mouse_y - radar_y;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if ( config_variables->misc.radar_enabled->value )
		{
			const int sw = ( int ) window_handler->window_size.x;
			const int sh = ( int ) window_handler->window_size.y;
			const int rs = config_variables->misc.radar_size->value;
			const int rx = ( int ) config_variables->misc.radar_x->value;
			const int ry = ( int ) config_variables->misc.radar_y->value;
			if ( mouse_x >= rx && mouse_x <= rx + rs + 4 &&
				 mouse_y >= ry && mouse_y <= ry + 28 + rs + 2 )
			{
				const int delta = GET_WHEEL_DELTA_WPARAM( w_param );
				float& zoom = config_variables->misc.radar_zoom->value;
				zoom += ( delta > 0 ? 0.1f : -0.1f );
				if ( zoom < 1.f ) zoom = 1.f;
				if ( zoom > 4.f ) zoom = 4.f;
			}
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		const int sw = ( int ) window_handler->window_size.x;
		const int sh = ( int ) window_handler->window_size.y;

		auto clamp_i = []( int val, int lo, int hi ) { return val < lo ? lo : ( val > hi ? hi : val ); };

		if ( is_dragging )
		{
			menu->position.x = clamp_i( mouse_x - drag_offset.x, 0, sw - ( int ) menu->size.x );
			menu->position.y = clamp_i( mouse_y - drag_offset.y, 0, sh - ( int ) menu->size.y );
		}

		if ( kbl_dragging )
		{
			config_variables->misc.keybind_list_x->value = ( float ) clamp_i( mouse_x - kbl_drag_offset.x, 0, sw - misc->kbl_w );
			config_variables->misc.keybind_list_y->value = ( float ) clamp_i( mouse_y - kbl_drag_offset.y, 0, sh - misc->kbl_h );
		}

		if ( spec_dragging )
		{
			config_variables->misc.spectator_list_x->value = ( float ) clamp_i( mouse_x - spec_drag_offset.x, 0, sw - 130 );
			config_variables->misc.spectator_list_y->value = ( float ) clamp_i( mouse_y - spec_drag_offset.y, 0, sh - 20 );
		}

		if ( bomb_dragging )
		{
			config_variables->misc.bomb_timer_x->value = ( float ) clamp_i( mouse_x - bomb_drag_offset.x, 0, sw - misc->bomb_w );
			config_variables->misc.bomb_timer_y->value = ( float ) clamp_i( mouse_y - bomb_drag_offset.y, 0, sh - misc->bomb_h );
		}

		if ( radar_dragging )
		{
			const int rs = config_variables->misc.radar_size->value;
			config_variables->misc.radar_x->value = ( float ) clamp_i( mouse_x - ( int ) radar_drag_offset.x, 0, sw - rs - 4 );
			config_variables->misc.radar_y->value = ( float ) clamp_i( mouse_y - ( int ) radar_drag_offset.y, 0, sh - 28 - rs );
		}
		break;
	}
	case WM_LBUTTONUP:
	{
		is_dragging = false;
		kbl_dragging = false;
		spec_dragging = false;
		bomb_dragging = false;
		radar_dragging = false;
		break;
	}
	}
}

void c_input_handler::handle_key_menu_input( UINT msg, WPARAM w_param, LPARAM l_param )
{
	switch ( msg )
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if ( w_param < 256 )
			current_state[ w_param ] = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if ( w_param < 256 )
			current_state[ w_param ] = false;
		break;
	case WM_LBUTTONDOWN:
		current_state[ VK_LBUTTON ] = true;
		break;
	case WM_LBUTTONUP:
		current_state[ VK_LBUTTON ] = false;
		break;
	case WM_RBUTTONDOWN:
		current_state[ VK_RBUTTON ] = true;
		break;
	case WM_RBUTTONUP:
		current_state[ VK_RBUTTON ] = false;
		break;
	case WM_MBUTTONDOWN:
		current_state[ VK_MBUTTON ] = true;
		break;
	case WM_MBUTTONUP:
		current_state[ VK_MBUTTON ] = false;
		break;
	case WM_XBUTTONDOWN:
		current_state[ GET_XBUTTON_WPARAM( w_param ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 ] = true;
		break;
	case WM_XBUTTONUP:
		current_state[ GET_XBUTTON_WPARAM( w_param ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 ] = false;
		break;
	case WM_MOUSEWHEEL:
		scroll_delta += GET_WHEEL_DELTA_WPARAM( w_param );
		break;
	}
}

char c_input_handler::key_state( int key )
{
	if ( key < 0 || key >= 256 )
		return NONE;

	bool curr = current_state[ key ];
	bool prev = previous_state[ key ];

	if ( curr && !prev )
		return PRESSED;

	if ( curr && prev )
		return HELD;

	if ( !curr && prev )
		return RELEASED;

	return NONE;
}

char c_input_handler::keybind_state( int key )
{
	if ( key < 0 || key >= 256 )
		return NONE;

	bool curr = current_keybinds_state[ key ];
	bool prev = previous_keybinds_state[ key ];

	if ( curr && !prev )
		return PRESSED;
	if ( curr && prev )
		return HELD;
	if ( !curr && prev )
		return RELEASED;
	return NONE;
}

std::string c_input_handler::get_key_name( int code )
{
	if ( code < 1 || code > 254 )
		return "";

	if ( code >= 'A' && code <= 'Z' ) return std::string( 1, ( char ) code );
	if ( code >= '0' && code <= '9' ) return std::string( 1, ( char ) code );

	if ( code >= VK_F1 && code <= VK_F12 )
		return "F" + std::to_string( code - VK_F1 + 1 );

	switch ( code )
	{
	case VK_LBUTTON: return "M1";
	case VK_RBUTTON: return "M2";
	case VK_MBUTTON: return "M3";
	case VK_XBUTTON1: return "M4";
	case VK_XBUTTON2: return "M5";

	case VK_ESCAPE:   return "ESC";
	case VK_SPACE:    return "SPACE";
	case VK_RETURN:   return "ENTER";
	case VK_BACK:     return "BACK";
	case VK_TAB:      return "TAB";
	case VK_SHIFT:    return "SHFIT";
	case VK_CONTROL:  return "CTRL";
	case VK_MENU:     return "ALT";
	case VK_CAPITAL:  return "CAPS";

	case VK_INSERT:   return "INS";
	case VK_DELETE:   return "DEL";
	case VK_HOME:     return "HOME";
	case VK_END:      return "END";
	case VK_PRIOR:    return "PGUP";
	case VK_NEXT:     return "PGDN";

	case VK_LEFT:     return "LEFT";
	case VK_RIGHT:    return "RGHT";
	case VK_UP:       return "UP";
	case VK_DOWN:     return "DOWN";

	case VK_NUMPAD0:  return "N0";
	case VK_NUMPAD1:  return "N1";
	case VK_NUMPAD2:  return "N2";
	case VK_NUMPAD3:  return "N3";
	case VK_NUMPAD4:  return "N4";
	case VK_NUMPAD5:  return "N5";
	case VK_NUMPAD6:  return "N6";
	case VK_NUMPAD7:  return "N7";
	case VK_NUMPAD8:  return "N8";
	case VK_NUMPAD9:  return "N9";
	case VK_DECIMAL:  return "N.";
	case VK_ADD:      return "N+";
	case VK_SUBTRACT: return "N-";
	case VK_MULTIPLY: return "N*";
	case VK_DIVIDE:   return "N/";

	default:
		return "";
	}
}

void c_input_handler::update_keybinds( )
{
	for ( auto& key : bound_keys )
	{
		if ( GetAsyncKeyState( key ) )
		{
			current_keybinds_state[ key ] = true;
		}
		else
		{
			current_keybinds_state[ key ] = false;
		}
	}
}

// this has to be the most retarded work around for how wndproc handles inputs
// bc of the transparent window we cant just check if we pressed a keyboard key
void c_input_handler::poll_keybinding( const float& global_time )
{
	if ( !menu->is_open )
		return;

	c_keybind_variable* target_var = nullptr;
	static c_keybind_variable* old_target_var = nullptr;
	static float next_allowed_press_time = 0.f;

	for ( auto& var : config_variables->all_variables ) 
	{
		if ( var->get_type( ) == config_var_type::keybind ) 
		{
			auto* kb = static_cast< c_keybind_variable* >( var );
			if ( kb->waiting_for_input )
			{
				target_var = kb;
				break;
			}
		}
	}

	if ( !target_var ) 
		return;

	// :exploding_head:
	if ( next_allowed_press_time <= 0.f && old_target_var != target_var ) 
	{
		next_allowed_press_time = global_time + 0.12f;
		old_target_var = target_var;
		return;
	}

	// l0l
	if ( next_allowed_press_time > global_time )
	{
		if ( GetAsyncKeyState( VK_LBUTTON ) || GetAsyncKeyState( VK_RBUTTON ) || GetAsyncKeyState( VK_MBUTTON ) )
		{
			next_allowed_press_time = global_time + 0.02f;
			return;
		}
	}

	if ( global_time < next_allowed_press_time || !old_target_var || !target_var )
		return;

	for ( int i = 1; i < 255; ++i ) 
	{
		if ( GetAsyncKeyState( i ) ) 
		{
			if ( i == VK_ESCAPE || i == VK_INSERT ) 
			{
				target_var->key = 0;
				target_var->waiting_for_input = false;
				return;
			}

			target_var->key = i;
			target_var->waiting_for_input = false;
			add_keybind( i );

			next_allowed_press_time = 0.f;
			old_target_var = nullptr;

			if ( target_var->previous_key > 0 )
				remove_keybind( target_var->previous_key );

			break;
		}
	}
}

bool c_input_handler::poll_text_input( const float& global_time )
{
	if ( !menu->is_open )
		return false;

	static float next_char_time = 0.f;
	c_text_variable* target_var = nullptr;
	for ( auto& var : config_variables->all_variables )
	{
		if ( var->get_type( ) == config_var_type::text )
		{
			auto* kb = static_cast< c_text_variable* >( var );
			if ( kb->is_typing )
			{
				target_var = kb;
				break;
			}
		}
	}

	if ( !target_var || !target_var->is_typing )
		return false;

	// skip the mouse shit, thats why i = 8
	for ( int i = 8; i < 255; ++i ) 
	{
		if ( GetAsyncKeyState( i ) & 1 ) 
		{
			if ( global_time < next_char_time ) 
				return false;

			next_char_time = global_time + 0.008f;

			if ( i == VK_BACK )
			{
				if ( !target_var->value.empty( ) )
					target_var->value.pop_back( );

				continue;
			}
			else if ( i == VK_SPACE )
			{
				if ( target_var->value.length( ) < target_var->max_length )
					target_var->value += ' ';
			}

			if ( i == VK_RETURN || i == VK_ESCAPE )
			{
				target_var->is_typing = false;
				return false;
			}

			if ( target_var->value.length( ) < target_var->max_length )
			{
				bool shift = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 );

				char c = ( char ) MapVirtualKeyA( i, MAPVK_VK_TO_CHAR );

				if ( c >= 32 && c <= 126 )
				{
					if ( !shift && c >= 'A' && c <= 'Z' )
						c += 32;

					target_var->value += c;
				}
			}
			else
				break;
		}
	}

	return target_var && target_var->is_typing;
}