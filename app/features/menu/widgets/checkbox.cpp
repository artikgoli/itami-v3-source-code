#include "checkbox.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\menu.h"

bool c_checkbox::on_input( vector2d pos, vector2d mouse )
{
    if ( toggle_function )
        render_enabled = toggle_function( );

    if ( !render_enabled )
        return false;

    if ( linked_keybind && linked_keybind->additional_options_open )
    {
        auto key_name = linked_keybind->waiting_for_input ? "[ ... ]" : "[ " + input_handler->get_key_name( linked_keybind->key ) + " ]";
        auto key_size = draw_pool->font_smooth.text_extent( key_name );

        int menu_x = pos.x + parent_widget->width - key_size.x - 14;
        int menu_y = pos.y + height + 2;
        int menu_width = 80;
        int item_height = 18;

        bool click = input_handler->key_state( VK_LBUTTON ) == key_state::PRESSED;

        if ( click )
        {
            for ( int i = 0; i < 3; i++ )
            {
                bool hovered_item = ( mouse.x >= menu_x && mouse.x <= menu_x + menu_width &&
                    mouse.y >= menu_y + ( i * item_height ) && mouse.y <= menu_y + ( ( i + 1 ) * item_height ) );

                if ( hovered_item )
                {
                    linked_keybind->activation_type = i;
                    linked_keybind->additional_options_open = false;
                    return true;
                }
            }
            linked_keybind->additional_options_open = false;
        }
        return true;
    }

    if ( menu->input_block_id > 0 && menu->input_block_id != this->unique_id )
        return false;

    hovered = ( mouse.x >= pos.x - 5 && mouse.x <= pos.x + width + 7 &&
        mouse.y >= pos.y && mouse.y <= pos.y + height );

    bool click = input_handler->key_state( VK_LBUTTON ) == key_state::PRESSED;
    bool click_m2 = input_handler->key_state( VK_RBUTTON ) == key_state::PRESSED;

    if ( hovered && click )
    {
        if ( linked_bool )
            *linked_bool = !*linked_bool;

        menu->input_block_id = this->unique_id;
    }

    if ( linked_keybind )
    {
        auto text_size = draw_pool->font_smooth.text_extent( display_name );
        std::string key_name = linked_keybind->waiting_for_input ? "[ ... ]" : "[ " + input_handler->get_key_name( linked_keybind->key ) + " ]";
        if ( key_name.empty( ) ) key_name = "[ - ]";

        auto key_size = draw_pool->font_smooth.text_extent( key_name );
        int keybind_x = pos.x + parent_widget->width - key_size.x - 14;
        int keybind_y = pos.y + 1;
        bool over_keybind = ( mouse.x >= keybind_x && mouse.x <= keybind_x + key_size.x + 8 &&
            mouse.y >= keybind_y && mouse.y <= keybind_y + height );

        if ( over_keybind )
        {
            if ( click )
                linked_keybind->waiting_for_input = true;
            else if ( click_m2 )
                linked_keybind->additional_options_open = !linked_keybind->additional_options_open;
        }
    }

    active = hovered && click;
    if ( active )
        menu->input_block_id = 0;

    return active;
}

void c_checkbox::on_render_post( vector2d pos )
{
    if ( linked_keybind && linked_keybind->additional_options_open )
    {
        std::string key_name = linked_keybind->waiting_for_input ? "[ ... ]" : "[ " + input_handler->get_key_name( linked_keybind->key ) + " ]";
        auto key_size = draw_pool->font_smooth.text_extent( key_name );

        int menu_x = pos.x + parent_widget->width - key_size.x - 14;
        int menu_y = pos.y + height + 2;
        int menu_width = 80;
        int item_height = 18;

        const char* modes[ ] = { "always", "hold", "toggle" };

        render->rect( menu_x, menu_y, menu_width, item_height * 3, color( 30, 30, 30 ), true );
        render->rect( menu_x - 1, menu_y - 1, menu_width + 1, ( item_height * 3 ) + 1, color( 5, 5, 5 ), false );

        for ( int i = 0; i < 3; i++ )
        {
            color text_clr = ( linked_keybind->activation_type == i ) ?
                config_variables->misc.checkbox_fill_clr->value : color( 200, 200, 200 );

            render->text( menu_x + 5, menu_y + ( i * item_height ) + 2,
                draw_pool->font_smooth, text_clr, daisy::TEXT_ALIGN_DEFAULT, true, modes[ i ] );
        }
    }
}

void c_checkbox::on_render( vector2d pos )
{
	color box_color = color( 35, 35, 35 );
	color check_color = config_variables->misc.checkbox_fill_clr->value;
	if ( hovered || active )
	{
		box_color = color( 45, 45, 45 );
	}

    render->rect( pos.x, pos.y, 14, 14, box_color, true );
    render->rect( pos.x, pos.y, 14, 14, color( 5, 5, 5 ), false );

    if (linked_bool && *linked_bool)
    {
        render->rect( pos.x + 1, pos.y + 1, 12, 12, check_color, true );
        render->gradient_rect( pos.x + 1, pos.y + 1, 12, 12, color( 0, 0, 0, 0 ), color( 0, 0, 0, 45 ), true );
    }

	auto text_size = draw_pool->font_smooth.text_extent( display_name );
	width = text_size.x + 14 + 15;

	render->text( pos.x + 14 + 6,
		pos.y + ( height - 2 ) / 2.f - text_size.y / 2.f,
		draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true,
		display_name.data( ) );

	// if we have a keybind, render it next to the checkbox
	if ( linked_keybind )
	{
		std::string key_name = linked_keybind->waiting_for_input ? "[ ... ]" : "[ " + input_handler->get_key_name( linked_keybind->key ) + " ]";
		if ( key_name.empty( ) )
			key_name = "[ - ]";

		auto key_size = draw_pool->font_smooth.text_extent( key_name );
		render->rect( pos.x + parent_widget->width - key_size.x - 14,
			pos.y + 2,
			key_size.x + 8,
			height - 2 + 1,
			color( 25, 25, 25 ), true );

		render->rect( pos.x + parent_widget->width - key_size.x - 1 - 14,
			pos.y + 2,
			key_size.x + 8,
			height - 2 + 1,
			color( 5, 5, 5 ), false );

		render->text( pos.x + parent_widget->width - key_size.x + 4 - 14,
			pos.y + ( height + 1 ) / 2.f - key_size.y / 2.f,
			draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true,
			key_name.data( ) );
	}
}
