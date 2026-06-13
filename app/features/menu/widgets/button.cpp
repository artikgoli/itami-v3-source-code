#include "button.h"
#include "..\menu.h"
#include "..\..\..\renderer\renderer.h"

// checkbox copy paste winning function #notgpt
bool c_button::on_input( vector2d pos, vector2d mouse )
{
	if ( toggle_function )
		render_enabled = toggle_function( );

	if ( !render_enabled )
		return false;

	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	hovered = ( mouse.x >= pos.x - 5 && mouse.x <= pos.x + width + 7 &&
		mouse.y >= pos.y + 5 && mouse.y <= pos.y + 20 + 5 );

	bool click = input_handler->key_state( VK_LBUTTON ) == key_state::PRESSED;

	if ( hovered && click )
	{
		button_func( );
		menu->input_block_id = this->unique_id;
	}

	active = hovered && click;
	if ( active )
		menu->input_block_id = 0;

	return active;
}

void c_button::on_render( vector2d pos )
{
	if ( !render_enabled )
		return;

	render->rect( pos.x, pos.y + 5, width, 20, color( 25, 25, 25 ), true );
	render->rect( pos.x - 1, pos.y - 1 + 5, width + 1, 20 + 1, color( 0, 0, 0 ), false );
	render->text( pos.x + width / 2.f, pos.y + 5 + 3, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGNX_CENTER, true, display_name.data( ) );
}
