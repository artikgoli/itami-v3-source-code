#include "dynamic_combobox.h"
#include "..\menu.h"
#include "..\..\..\renderer\renderer.h"

void c_dynamic_combobox::on_render( vector2d pos )
{
	width = parent_widget->width - 10;

	render->rect( pos.x, pos.y + 14, width, 16, color( 50, 50, 50, 255 ), true );
	render->rect( pos.x, pos.y + 13, width, 17, color( 0, 0, 0 ), false );
	render->text( pos.x + 1, pos.y - 4, draw_pool->font_smooth, color( 255, 255, 255 ),
		daisy::TEXT_ALIGN_DEFAULT, true, "%s", display_name.data( ) );

	if ( ( *items ).empty( ) )
		return;

	render->text( pos.x + 6, pos.y + 13, draw_pool->font_smooth, color( 255, 255, 255 ),
		daisy::TEXT_ALIGN_DEFAULT, true, ( *items )[ *config_var ].data( ) );
}

void c_dynamic_combobox::on_render_post( vector2d pos )
{
	if ( is_open )
	{
		int offset_y = 0;

		render->rect( pos.x, pos.y + 30, width, ( *items ).size( ) * 16, color( 35, 35, 35, 255 ), true );
		render->rect( pos.x, pos.y + 30, width, ( *items ).size( ) * 16, color( 0, 0, 0, 255 ), false );

		for ( size_t i = 0; i < ( *items ).size( ); ++i )
		{
			int item_y = pos.y + 30 + offset_y;
			bool is_selected = ( *config_var == ( int ) i );

			render->rect( pos.x + 6, item_y + 3, 10, 10, color( 35, 35, 35 ), true );
			render->rect( pos.x + 6, item_y + 3, 10, 10, color( 0, 0, 0 ), false );

			if ( is_selected )
			{
				render->rect( pos.x + 8, item_y + 5, 7, 7, config_variables->misc.checkbox_fill_clr->value, true );
				render->gradient_rect( pos.x + 8, item_y + 5, 7, 7, color( 0, 0, 0, 0 ), color( 0, 0, 0, 60 ), true );
			}

			render->text( pos.x + 20, item_y, draw_pool->font_smooth, 
				is_selected ? color( 255, 255, 255 ) : color( 180, 180, 180 ), daisy::TEXT_ALIGN_DEFAULT, true, ( *items )[ i ].data( ) );

			offset_y += 16;
		}
	}
}

bool c_dynamic_combobox::on_input( vector2d pos, vector2d mouse )
{
	// todo: add on hover color change to the background
	// change the background color to match a bit more the menu style

	if ( toggle_function )
		render_enabled = toggle_function( );

	if ( !render_enabled )
		return false;

	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	if ( input_handler->key_state( VK_LBUTTON ) == PRESSED )
	{
		if ( is_open )
		{
			int offset_y = 0;
			for ( size_t i = 0; i < ( *items ).size( ); i++ )
			{
				int item_y = pos.y + 14 + 16 + offset_y;
				if ( mouse.x >= pos.x && mouse.x <= pos.x + width &&
					mouse.y >= item_y && mouse.y <= item_y + 16 )
				{
					*config_var = ( int ) i;
					is_open = false;
					return true;
				}

				offset_y += 16;
			}
		}

		if ( mouse.x >= pos.x && mouse.x <= pos.x + width - 5 &&
			mouse.y >= pos.y + 14 && mouse.y <= pos.y + 14 + 16 )
		{
			is_open = !is_open;
			menu->input_block_id = this->unique_id;
			return true;
		}
	}

	menu->input_block_id = 0;
	return false;
}
