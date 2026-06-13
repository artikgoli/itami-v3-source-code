#pragma once
#include "base_widget.h"

class c_dynamic_combobox : public c_base_widget
{
public:
	c_dynamic_combobox( std::string name, int* config_var, std::vector<std::string>* items, toggle_function_fn toggle )
		: items( items ), config_var( config_var )
	{
		display_name = name;
		type = widget_type::WIDGET_TYPE_COMBOBOX;
		height = 14 + 18;
		render_enabled = true;
		toggle_function = toggle;
	}
	void on_render( vector2d position ) override;
	void on_render_post( vector2d position ) override;
	bool on_input( vector2d pos, vector2d mouse ) override;

	std::vector<std::string>* items = {};
	int* config_var = nullptr;
	bool is_open = false;
};