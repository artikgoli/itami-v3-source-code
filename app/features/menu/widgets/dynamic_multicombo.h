#pragma once
#include "base_widget.h"

struct c_dynamic_multicombo_item
{
	std::string display_name {};
	bool* config_var = {};

	c_dynamic_multicombo_item( std::string display_name, bool* config_var ) :
		display_name( display_name ), config_var( config_var )
	{

	}
};

class c_dynamic_multicombo : public c_base_widget
{
public:
	c_dynamic_multicombo( std::string name, std::vector<c_dynamic_multicombo_item> _items, toggle_function_fn toggle )
	{
		display_name = name;
		height = 30;
		type = widget_type::WIDGET_TYPE_DYNAMIC_MULTICOMBOBOX;
		width = 140;
		render_enabled = true;
		items = _items;
		toggle_function = toggle;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override;

	bool is_open = false;
	std::vector<c_dynamic_multicombo_item> items {};
};