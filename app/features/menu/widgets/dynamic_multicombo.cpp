#include "dynamic_multicombo.h"
#include "..\..\..\renderer\renderer.h"
#include "..\..\..\renderer\window_handler.h"
#include "..\input\input.h"
#include "..\menu.h"

void c_dynamic_multicombo::on_render( vector2d pos )
{
	width = parent_widget->width - 10;

	render->rect( pos.x, pos.y + 14, width, 16, color( 50, 50, 50, 255 ), true );
	render->rect( pos.x, pos.y + 13, width, 17, color( 0, 0, 0 ), false );
	render->text( pos.x + 1, pos.y - 4, draw_pool->font_smooth, color( 255, 255, 255 ),
		daisy::TEXT_ALIGN_DEFAULT, true, "%s", display_name.data( ) );

	std::string display_data = {};
	for ( auto& item : items )
		if ( *item.config_var )
			display_data += item.display_name + ", ";

	if ( display_data.size( ) > 3 )
	{
		display_data.pop_back( );
		display_data.pop_back( );
	}

	render->set_viewport( pos.x + 1, pos.y + 14 + 1, width - 2, 16 - 2 );

	int y_offset = 14 + 8 - draw_pool->font_smooth.text_extent( display_data ).y / 2.f;
	if ( !( pos.y + y_offset > menu->size.y - 10 + menu->position.y ) )
	{
		render->text( pos.x + 6, pos.y + y_offset, draw_pool->font_smooth, color( 255, 255, 255 ),
			daisy::TEXT_ALIGN_DEFAULT, true,
			display_data.data( ) );
	}

	render->restore_viewport( );
}

bool c_dynamic_multicombo::on_input( vector2d pos, vector2d mouse )
{
	if ( toggle_function )
		render_enabled = toggle_function( );

	if ( !render_enabled )
		return false;

	if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
		return false;

	if ( input_handler->key_state( VK_LBUTTON ) == key_state::PRESSED )
	{
		if ( is_open )
		{
			for ( size_t i = 0; i < items.size( ); ++i )
			{
				int item_y = pos.y + 30 + ( i * 16 );
				if ( mouse.x > pos.x && mouse.x < pos.x + width &&
					mouse.y > item_y && mouse.y < item_y + 16 )
				{
					*items[ i ].config_var = !( *items[ i ].config_var );
					menu->input_block_id = this->unique_id;
					return true;
				}
			}
		}

		if ( mouse.x > pos.x && mouse.x < pos.x + width &&
			mouse.y > pos.y + 14 && mouse.y < pos.y + 30 )
		{
			is_open = !is_open;
			if ( is_open )
				menu->input_block_id = this->unique_id;
			else
				menu->input_block_id = 0;

			return true;
		}
	}

	menu->input_block_id = 0;
	return false;
}

void c_dynamic_multicombo::on_render_post( vector2d pos )
{
	if ( is_open )
	{
		int y_offset = 0;
		int dropdown_y_start = pos.y + 30;

		render->rect( pos.x, dropdown_y_start, width, items.size( ) * 16, color( 35, 35, 35, 255 ), true );
		render->rect( pos.x, dropdown_y_start, width, items.size( ) * 16, color( 0, 0, 0, 255 ), false );

		for ( size_t i = 0; i < items.size( ); ++i )
		{
			int current_item_y = dropdown_y_start + y_offset;
			bool is_active = items[ i ].config_var && *items[ i ].config_var;

			render->rect( pos.x + 6, current_item_y + 3, 10, 10, color( 35, 35, 35 ), true );
			render->rect( pos.x + 6, current_item_y + 3, 10, 10, color( 0, 0, 0 ), false );

			if (is_active)
			{
				render->rect( pos.x + 8, current_item_y + 5, 7, 7, config_variables->misc.checkbox_fill_clr->value, true );
				render->gradient_rect( pos.x + 8, current_item_y + 5, 7, 7, color( 0, 0, 0, 0 ), color( 0, 0, 0, 60 ), true );
			}

			render->text( pos.x + 22, current_item_y + 8 - draw_pool->font_smooth.text_extent( items[ i ].display_name ).y / 2.f,
				draw_pool->font_smooth, is_active ? color( 255, 255, 255 ) : color( 180, 180, 180 ),
				daisy::TEXT_ALIGN_DEFAULT, true,
				items[ i ].display_name.data( ) );

			y_offset += 16;
		}
	}
}
