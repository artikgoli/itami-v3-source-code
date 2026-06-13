#include "feature_seperator.h"
#include "..\..\..\renderer\renderer.h"
#include "colorpicker.h"

void c_feature_seperator::on_render( vector2d pos )
{
	width = parent_widget->width;

	auto clr = config_variables->misc.menu_outline_color->value;
	auto clr_a = color( clr.r, clr.g, clr.b, 90 );

	auto text_size = draw_pool->font_smooth.text_extent( display_name );
	render->gradient_line( pos.x, pos.y, pos.x + ( width - text_size.x ) / 2.f - 4, pos.y,
		clr_a, clr, clr, 1.f );

	render->gradient_line( pos.x + ( width + text_size.x ) / 2.f + 4, pos.y, pos.x + width, pos.y,
		clr, clr, clr_a, 1.f );

	render->text( pos.x + width / 2.f, pos.y - text_size.y / 2.f + 1,
		draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGNX_CENTER, true,
		display_name.data( ) );

	int offset_y = 10;
	int offset_x = 0;

	for ( size_t i = 0; i < children.size( ); i++ )
	{
		auto& child = children[ i ];

		if ( !child->render_enabled || child->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			continue;

		bool next_is_same_line = false;
		if ( i + 1 < children.size( ) && children[ i + 1 ]->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			next_is_same_line = true;
		
		child->on_render( vector2d( pos.x + offset_x + 5, pos.y + offset_y ) );

		if ( next_is_same_line )
			offset_x += child->width + 5;
		else
		{
			if ( offset_x != 0 )
				offset_y += 5;

			offset_x = 0;
			offset_y += child->height + 5;
		}
	}

	height = offset_y + 5;
}

void c_feature_seperator::on_render_post( vector2d pos )
{
	int offset_y = 10;
	int offset_x = 0;

	for ( size_t i = 0; i < children.size( ); i++ )
	{
		auto& child = children[ i ];

		if ( !child->render_enabled || child->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			continue;

		if ( child->type == WIDGET_TYPE_COLORPICKER )
		{
			auto clr_pick = ( c_colorpicker* ) ( child.get( ) );
			if ( clr_pick->is_open && clr_pick != last_open_colorpicker )
				clr_pick->is_open = false;
		}

		bool next_is_same_line = false;
		if ( i + 1 < children.size( ) && children[ i + 1 ]->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			next_is_same_line = true;

		child->on_render_post( vector2d( pos.x + offset_x + 5, pos.y + offset_y ) );

		if ( next_is_same_line )
			offset_x += child->width + 5;
		else
		{
			if ( offset_x != 0 )
				offset_y += 5;

			offset_x = 0;
			offset_y += child->height + 5;
		}
	}
}

bool c_feature_seperator::on_input( vector2d pos, vector2d mouse )
{
	int offset_y = 10;
	int offset_x = 0;
	for ( size_t i = 0; i < children.size( ); i++ )
	{
		auto& child = children[ i ];

		bool ret = child->on_input( vector2d( pos.x + offset_x, pos.y + offset_y ), mouse );
		if ( !child->render_enabled || child->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			continue;

		bool next_is_same_line = false;
		if ( i + 1 < children.size( ) && children[ i + 1 ]->type == WIDGET_TYPE_SAME_LINE_SEPERATOR )
			next_is_same_line = true;

		if ( next_is_same_line )
			offset_x += child->width + 5;
		else
		{
			if ( offset_x != 0 )
				offset_y += 5;

			offset_y += child->height + 5;
			offset_x = 0;
		}

		if ( ret )
			return true;
	}

	return false;
}
