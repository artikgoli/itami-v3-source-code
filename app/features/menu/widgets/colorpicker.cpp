#include "colorpicker.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\menu.h"

/*
	TODO: ALPHA SLIDER AT THE BOTTOM
	MAKE THE MENU CLOSABLE BY CLICKING OUTSIDE OF IT
	FOCUS THE MENU WHEN CLICKING ON IT
*/

void c_colorpicker::on_render( vector2d pos )
{
	// draw string + preview
	render->text( pos.x, pos.y - 1, draw_pool->font_smooth, color( 255, 255, 255 ),
		daisy::TEXT_ALIGN_DEFAULT, true, display_name.data( ) );

	auto text_size = draw_pool->font_smooth.text_extent( display_name );

	render->rect( pos.x + text_size.x + 15 - 1, pos.y + 3 - 1, 24 + 2, 8 + 2, color( 0, 0, 0 ), true );
	render->rect( pos.x + text_size.x + 15, pos.y + 3, 24, 8, *config_var, true );
}

bool c_colorpicker::on_input( vector2d pos, vector2d mouse )
{
	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	auto text_size = draw_pool->font_smooth.text_extent( display_name );

	if ( input_handler->key_state( VK_LBUTTON ) == PRESSED )
	{
		if ( mouse.x >= pos.x + text_size.x + 15 && mouse.x <= pos.x + text_size.x + 15 + 24 &&
			mouse.y >= pos.y + 3 && mouse.y <= pos.y + 3 + 8 )
		{
			is_open = !is_open;
			if ( is_open )
			{
				if ( parent_widget && parent_widget->type == WIDGET_TYPE_FEATURE_SEPERATOR )
					( ( c_feature_seperator* ) ( parent_widget.get( ) ) )->last_open_colorpicker = this;
			}
			return true;
		}
	}

	int picker_x = ( int ) pos.x - 128 - 45;
	int picker_y = ( int ) pos.y;
	int picker_width = 128;
	int picker_height = 128;

	int slider_x = picker_x + picker_width + 5;
	int slider_w = 10;

	if ( is_open && input_handler->key_state( VK_LBUTTON ) == HELD ) {

		if ( mouse.x >= slider_x && mouse.x <= ( slider_x + slider_w ) &&
			mouse.y >= picker_y - 5 && mouse.y <= ( picker_y + picker_height + 10 ) )
		{
			this->hue_slider = std::clamp( mouse.y - ( float ) picker_y, 0.f, ( float ) picker_height );

			float h = ( this->hue_slider / ( float ) picker_height ) * 360.f;
			float hsv[ 3 ];
			float old_alpha = config_var->a;
			color::rgb_to_hsv( *config_var, hsv );
			*config_var = color::hsv_to_rgb( h, hsv[ 1 ], hsv[ 2 ] );
			config_var->a = old_alpha;

			menu->input_block_id = this->unique_id;
			return true;
		}
		else if ( mouse.x >= picker_x && mouse.x <= ( picker_x + picker_width ) &&
			mouse.y >= picker_y && mouse.y <= ( picker_y + picker_height ) )
		{
			float h = ( this->hue_slider / ( float ) picker_height ) * 360.f;
			float s = std::clamp( ( mouse.x - picker_x ) / ( float ) picker_width, 0.f, 1.f );
			float v = 1.f - std::clamp( ( mouse.y - picker_y ) / ( float ) picker_height, 0.f, 1.f );

			float old_alpha = config_var->a;
			*config_var = color::hsv_to_rgb( h, s, v );
			config_var->a = old_alpha;
			menu->input_block_id = this->unique_id;
			return true;
		}
		else if ( mouse.x >= picker_x + 5 - 5 && mouse.x <= picker_x + 5 + picker_width - 4 + 10 &&
			mouse.y >= picker_y + picker_height + 15 && mouse.y <= picker_y + picker_height + 15 + 11 )
		{
			float alpha_ratio = std::clamp( ( mouse.x - ( picker_x + 5 ) ) / ( float ) ( picker_width - 4 ), 0.f, 1.f );
			color current_color = *config_var;
			current_color.a = ( int ) ( alpha_ratio * 255.f );
			*config_var = current_color;
			menu->input_block_id = this->unique_id;
			return true;
		}
	}

	menu->input_block_id = 0;
	return is_open;
}

void c_colorpicker::on_render_post( vector2d pos )
{
	color::rgb_to_hsv( *config_var, hsv );
	hue_slider = ( hsv[ 0 ] / 360.f ) * 128.f;

	if ( is_open )
	{
		int picker_x = pos.x - 128 - 45;
		int picker_y = pos.y;

		int picker_width = 128;
		int picker_height = 128;

		static color hue_colors[ 13 ] = {
			{ 255, 0, 0 }, { 255, 128, 0 }, { 255, 255, 0 }, { 128, 255, 0 },
			{ 0, 255, 0 }, { 0, 255, 128 }, { 0, 255, 255 }, { 0, 128, 255 },
			{ 0, 0, 255 }, { 128, 0, 255 }, { 255, 0, 255 }, { 255, 0, 128 }, { 255, 0, 0 }
		};

		float hue = ( hue_slider / ( float ) picker_height ) * 360.f;
		color square_color = color::hsv_to_rgb( hue, 1.f, 1.f );

		auto text_size = draw_pool->font_smooth.text_extent( display_name );
		int header_height = text_size.x > 0 ? 24 : 2;

		render->rect( picker_x - 2, picker_y - header_height, picker_width + 10 + 10, picker_height + header_height + 16 + 20, color( 10, 10, 10, 255 ), true );

		if ( text_size.x > 0 )
		{
			render->text( picker_x + 5 + picker_width / 2.f, picker_y - 20, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGNX_CENTER, true, display_name.data( ) );
		}

		float segment_height = ( float ) picker_height / 12.f;
		for ( int i = 0; i < 12; i++ )
		{
			render->gradient_rect(
				picker_x + picker_width + 5,
				picker_y + ( int ) ( i * segment_height ),
				10, ceilf( segment_height ),
				hue_colors[ i ], hue_colors[ i + 1 ],
				true );
		}

		render->gradient_rect( picker_x, picker_y, picker_width, picker_height, color( 255, 255, 255 ), square_color, false );
		render->gradient_rect( picker_x, picker_y, picker_width, picker_height, color( 0, 0, 0, 0 ), color( 0, 0, 0, 255 ), true );

		int slider_sel_y = picker_y + ( int ) hue_slider;
		render->rect( picker_x + picker_width + 4, slider_sel_y - 2, 12, 4, color( 0, 0, 0, 255 ), true );
		render->rect( picker_x + picker_width + 5, slider_sel_y - 1, 10, 2, color( 255, 255, 255, 255 ), true );

		float temp_hsv[ 3 ] { };
		color::rgb_to_hsv( *config_var, temp_hsv );
		int cursor_x = picker_x + ( int ) ( temp_hsv[ 1 ] * picker_width );
		int cursor_y = picker_y + ( int ) ( ( 1.f - temp_hsv[ 2 ] ) * picker_height );

		render->rect( cursor_x - 3, cursor_y - 3, 5, 5, color( 255, 255, 255 ), false );

		render->rect( cursor_x - 3 - 1, cursor_y - 3 - 1, 5 + 2, 5 + 2, color( 0, 0, 0 ), false );
		render->rect( cursor_x - 3 + 1, cursor_y - 3 + 1, 5 - 2, 5 - 2, color( 0, 0, 0 ), false );

		render->text( picker_x + 5, picker_y + picker_height + 1, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, "alpha" );
		render->rect( picker_x + 5, picker_y + picker_height + 15, picker_width - 4, 11, color( 35, 35, 35 ), true );
		render->rect( picker_x + 6, picker_y + picker_height + 16, picker_width - 6, 9, *config_var, true );

		float alpha_ratio = ( *config_var ).a / 255.f;
		render->rect( picker_x + picker_width * alpha_ratio - 1, picker_y + picker_height + 14, 1, 12, color( 255, 255, 255 ), false );
		render->rect( picker_x + picker_width * alpha_ratio - 1, picker_y + picker_height + 14, 2, 12, color( 0, 0, 0 ), false );
	}
}
