#pragma once
#include "base_widget.h"

class c_slider_int : public c_base_widget
{
public:
	c_slider_int( const std::string& name, int* config_var, int min_value, int max_value, toggle_function_fn toggle )
		: config_var( config_var ), min_value( min_value ), max_value( max_value )
	{
		display_name = name;
		width = 460.f / 2.f - 16;
		height = 24;
		type = widget_type::WIDGET_TYPE_SLIDER;
		render_enabled = true;
		toggle_function = toggle;
	}

	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override
	{
	}
	bool on_input( vector2d pos, vector2d mouse ) override;

	int* config_var {};
	int min_value {};
	int max_value {};
};
