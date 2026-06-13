#include "text_input.h"
#include "..\..\..\renderer\renderer.h"
#include "..\input\input.h"
#include "..\..\..\renderer\window_handler.h"

bool c_text_input::on_input( vector2d pos, vector2d mouse )
{
	if ( mouse.x >= pos.x && mouse.x <= pos.x + width &&
		mouse.y >= pos.y + 18 && mouse.y <= pos.y + 18 + 15 ) // height of the thingy u kno
	{
		if ( input_handler->key_state( VK_LBUTTON ) == PRESSED )
		{
			for ( auto& var : config_variables->all_variables )
			{
				if ( var->get_type( ) == config_var_type::text )
					static_cast< c_text_variable* >( var )->is_typing = false;
			}

			this->text_variable->is_typing = true;
			return true;
		}
	}

	return false;
}

void c_text_input::on_render( vector2d pos )
{
	if ( !render_enabled )
		return;

	bool is_typing = this->text_variable->is_typing;
	width = parent_widget->width - 10;

	render->text( pos.x, pos.y, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, display_name.data( ) );

	render->rect( pos.x, pos.y + 18, width, 15, color( 25, 25, 25 ), true );
	render->rect( pos.x - 1, pos.y + 18 - 1, width + 1, 15 + 1, is_typing ? config_variables->misc.menu_outline_color->value : color( 0, 0, 0 ), false );

	render->set_viewport( pos.x, pos.y + 18, width, 16 ); // people can be retarded

	std::string text_data = this->text_variable->value;
	if ( is_typing )
		text_data.append( "_" );

	render->text( pos.x + 2, pos.y + 18, draw_pool->font_smooth, color( 255, 255, 255 ),
		daisy::TEXT_ALIGN_DEFAULT, true, text_data.data( ) );

	render->restore_viewport( );
}
