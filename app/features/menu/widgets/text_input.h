#pragma once
#include "base_widget.h"

class c_text_input : public c_base_widget
{
public:
	c_text_input( const std::string& name, c_text_variable* menu_var, toggle_function_fn toggle )
	{
		display_name = name;
		width = 205;
		height = 30;
		type = WIDGET_TYPE_TEXT_INPUT;
		text_variable = menu_var;
		toggle_function = toggle;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override
	{
	};

	
	c_text_variable* text_variable = {};
};