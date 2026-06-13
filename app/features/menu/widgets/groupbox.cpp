#include "groupbox.h"
#include "..\..\..\renderer\renderer.h"
#include "..\menu.h"
#include "..\..\..\renderer\window_handler.h"

c_groupbox::c_groupbox( )
{
	display_name = "groupbox bro what did u expect";
	type = widget_type::WIDGET_TYPE_GROUPBOX;
	render_enabled = true;
}

void c_groupbox::on_render( vector2d pos )
{
	width = menu->size.x / 2.f - 16;
	height = menu->size.y - 38;

	int box_x = ( int ) pos.x;
	int box_y = ( int ) pos.y;

	int box_height = height;
	int box_width = width;

	render->rect( box_x - 5, box_y - 5, box_width + 10, box_height + 10, config_variables->misc.groupbox_outline_color->value, false );
	render->rect( box_x - 6, box_y - 6, box_width + 12, box_height + 12, color( 0, 0, 0 ), false );

	render->set_viewport( box_x, box_y - 4, width, height + 6 );
	render->save_viewport( box_x, box_y - 4, width, height + 6 );
	// save it here, otherwise children will overwrite it back to full-screen viewport

	int child_offset = 2 + ( int ) scroll_height;
	for ( auto& child : children )
	{
		child->on_render( vector2d( box_x, box_y + child_offset ) );
		child_offset += child->height + 10;
	}

	// restore global
	render->save_viewport( 0, 0, ( int ) daisy::daisy_t::s_viewport_width, ( int ) daisy::daisy_t::s_viewport_height );
	render->restore_viewport( );
}

void c_groupbox::on_render_post( vector2d pos )
{
	int child_offset = 2 + ( int ) scroll_height;
	for ( auto& child : children )
	{
		child->on_render_post( vector2d( pos.x, pos.y + child_offset ) );
		child_offset += child->height + 10;
	}
}

bool c_groupbox::on_input( vector2d pos, vector2d mouse )
{
	float total_content_height = 0.f;
	for ( auto& child : children )
		total_content_height += child->height + 10;

	if ( mouse.x >= pos.x && mouse.x <= pos.x + width &&
		mouse.y >= pos.y && mouse.y <= pos.y + height )
	{
		if ( input_handler->get_scroll_delta( ) != 0 )
		{
			scroll_height += input_handler->get_scroll_delta( ) / 6.f;

			float max_scroll = max( 0.f, total_content_height - height + 10.f );

			if ( scroll_height > 0.f )
				scroll_height = 0.f;

			if ( scroll_height < -max_scroll )
				scroll_height = -max_scroll;

			return true;
		}
	}

	int child_offset = 2 + ( int ) scroll_height;
	for ( auto& child : children )
	{
		if ( child->on_input( vector2d( pos.x, pos.y + child_offset ), mouse ) )
			return true;

		child_offset += child->height + 10;
	}
	return false;
}
