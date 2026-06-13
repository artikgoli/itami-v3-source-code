#include "tab_selector.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\menu.h"

bool c_tab_selector::on_input( vector2d pos, vector2d mouse )
{
	bool click = input_handler->key_state( VK_LBUTTON ) == key_state::PRESSED;
	hovered = ( mouse.x >= pos.x - 3 && mouse.x <= pos.x + width &&
		mouse.y >= pos.y && mouse.y <= pos.y + height + 2 );

	if ( hovered && click )
	{
		if ( menu_index )
			*menu_index = internal_index;
	}

	active = hovered && click;
	return active;
}

void c_tab_selector::on_render( vector2d pos )
{
	color outline_clr = config_variables->misc.menu_outline_color->value;
	render->gradient_line(pos.x - 3, pos.y + height + 2, pos.x + width, pos.y + height + 2,
		color( outline_clr.r, outline_clr.g, outline_clr.b, 90 ), outline_clr, color( outline_clr.r, outline_clr.g, outline_clr.b, 90 ), 1.f);

	auto size = draw_pool->font_smooth.text_extent( display_name );

	// this is so ghetto bro :heart:
	bool is_selected = this->internal_index == menu->selected_tab;

	render->text( pos.x + width / 2.f, 
		pos.y + height / 2.f - size.y / 2.f + 1,
		draw_pool->font_smooth, is_selected ? outline_clr : color( 255, 255, 255 ), daisy::TEXT_ALIGNX_CENTER, true, display_name.data( ) );
}
