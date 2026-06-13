#pragma once
#include "base_widget.h"

class c_checkbox : public c_base_widget
{
public:
	c_checkbox( const std::string& name, bool* config_var, c_keybind_variable* keybind, toggle_function_fn toggle = nullptr )
	{
		display_name = name;
		linked_bool = config_var;
		width = 14;
		height = 16;
		type = WIDGET_TYPE_CHECKBOX;
		linked_keybind = keybind;
		render_enabled = true;
		toggle_function = toggle;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override;
	bool* linked_bool = nullptr;
	c_keybind_variable* linked_keybind = nullptr;
};