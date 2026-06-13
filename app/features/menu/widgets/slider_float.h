#pragma once
#include "base_widget.h"

class c_slider_float : public c_base_widget
{
public:
	c_slider_float( const std::string& name, float* config_var, float min_value, float max_value, float step, toggle_function_fn toggle )
		: config_var( config_var ), min_value( min_value ), max_value( max_value ), step( step )
	{
		display_name = name;
		width = 120;
		height = 24;
		type = widget_type::WIDGET_TYPE_SLIDER;
		render_enabled = true;
		toggle_function = toggle;
	}

	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override
	{
	};
	bool on_input( vector2d pos, vector2d mouse ) override;

	float* config_var {};
	float min_value {};
	float max_value {};
	float step { 0.1f };
};
