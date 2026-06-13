#pragma once
#include "base_widget.h"

class c_button : public c_base_widget
{
public:
	c_button( const std::string& name, button_function_fn button_call, toggle_function_fn toggle, int custom_size )
	{
		display_name = name;
		if ( custom_size > 0 )
			width = custom_size;
		else 
			width = 100;

		height = 20;
		type = WIDGET_TYPE_BUTTON;
		render_enabled = true;
		toggle_function = toggle;
		button_func = button_call;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override
	{
	};

	button_function_fn button_func = nullptr;
};