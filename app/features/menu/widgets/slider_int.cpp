#include "slider_int.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\menu.h"

void c_slider_int::on_render( vector2d pos )
{
	int offset_y = 10;

	width = parent_widget->width - 10;
	// dont even ask bro V2 recode
	int inner_width = width - 1;
	int range = max_value - min_value;

	int filled_width = 0;
	if (range > 0)
		filled_width = std::clamp< int > ( ( ( *config_var - min_value ) * inner_width + ( range - 1 ) ) / range, 0, inner_width );

	color fill_color = config_variables->misc.slider_fill_color->value;

	render->rect( pos.x, pos.y + offset_y, width, 10, color( 0, 0, 0, 220 ), false );
	render->rect( pos.x + 1, pos.y + offset_y + 1, inner_width + 1, 9, color( 25, 25, 25 ), true );

	if (filled_width > 0)
	{
		render->rect( pos.x + 1, pos.y + offset_y + 1, filled_width, 9, fill_color, true );
		render->gradient_rect( pos.x + 1, pos.y + offset_y + 1, filled_width, 9, color( 0, 0, 0, 0 ), color( 0, 0, 0, 80 ), true );
	}

	render->text( pos.x + 1, pos.y - 6, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, "%s: %i", display_name.data( ), *config_var );
}

bool c_slider_int::on_input( vector2d pos, vector2d mouse )
{
	if ( toggle_function )
		render_enabled = toggle_function( );

	if ( !render_enabled )
		return false;

	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	int offset_y = 10;

	if ( *config_var < min_value )
		*config_var = min_value;
	if ( *config_var > max_value )
		*config_var = max_value;

	if ( mouse.x >= pos.x - 5 && mouse.x <= pos.x + width + 10 &&
		mouse.y >= pos.y + offset_y - 5 && mouse.y <= pos.y + offset_y + 10 )
	{
		if ( input_handler->key_state( VK_LBUTTON ) == key_state::HELD )
		{
			float range = max_value - min_value;
			float relative_x = mouse.x - pos.x;
			int new_value = static_cast<int>( min_value + ( relative_x / width ) * range );

			if ( new_value < min_value )
				new_value = min_value;
			if ( new_value > max_value )
				new_value = max_value;

			*config_var = new_value;
			menu->input_block_id = this->unique_id;
		}
		return true;
	}

	if ( input_handler->key_state( VK_LBUTTON ) != key_state::HELD )
		menu->input_block_id = 0;

	return false;
}
