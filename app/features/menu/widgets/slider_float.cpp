#include "slider_float.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\menu.h"

void c_slider_float::on_render( vector2d pos )
{
	int offset_y = 10;
	width = parent_widget->width - 10;
	// dont even ask bro TM
	float range = max_value - min_value;

	float t = range > 0.f ? ( *config_var - min_value ) / range : 0.f;
	t = std::clamp( t, 0.f, 1.f );

	if (t > 0.9999f)
		t = 1.f;

	int inner_width = width - 2;
	int filled_width = static_cast< int >( t * inner_width );

	color fill_color = config_variables->misc.slider_fill_color->value;

	render->rect( pos.x, pos.y + offset_y, width, 10, color( 0, 0, 0, 220 ), false );
	render->rect( pos.x + 1, pos.y + offset_y + 1, inner_width + 1, 9, color( 25, 25, 25 ), true );

	if (filled_width > 0)
	{
		render->rect( pos.x + 1, pos.y + offset_y + 1, filled_width, 9, fill_color, true );
		render->gradient_rect( pos.x + 1, pos.y + offset_y + 1, filled_width, 9, color( 0, 0, 0, 0 ), color( 0, 0, 0, 80 ), true );
	}

	// dont ask bro
	int precision = max( 0, ( int ) std::ceil( -std::log10( step ) ) );
	char fmt[ 16 ];
	snprintf( fmt, sizeof( fmt ), "%%s: %%.%df", precision );

	render->text( pos.x + 1, pos.y - 6, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGNX_LEFT, true, fmt, display_name.data( ), *config_var );
}

bool c_slider_float::on_input( vector2d pos, vector2d mouse )
{
	if ( toggle_function )
		render_enabled = toggle_function( );

	if ( !render_enabled )
		return false;

	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	int offset_y = 10;
	if ( mouse.x >= pos.x - 5 && mouse.x <= pos.x + width + 10 &&
		mouse.y >= pos.y + offset_y - 5 && mouse.y <= pos.y + offset_y + 10 )
	{
		if ( input_handler->key_state( VK_LBUTTON ) == key_state::HELD )
		{
			float range = max_value - min_value;
			float relative_x = mouse.x - pos.x;
			float new_value = min_value + ( relative_x / width ) * range;

			new_value = roundf( new_value / step ) * step;

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
