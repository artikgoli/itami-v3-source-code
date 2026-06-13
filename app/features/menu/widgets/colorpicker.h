#pragma once
#include "base_widget.h"

class c_colorpicker : public c_base_widget
{
public:
	c_colorpicker(const std::string& name, color* config, toggle_function_fn toggle )
	{
		display_name = name;
		config_var = config;
		width = 24;
		height = 12;
		render_enabled = true;
		type = widget_type::WIDGET_TYPE_COLORPICKER;
		toggle_function = toggle;
	}

	void on_render( vector2d pos ) override;
	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render_post( vector2d pos ) override;

	float hue_slider = 0.f;
	float hsv[ 3 ] = {};

	vector2d picker_pos = {};

	color* config_var = {};
	bool is_open = false;
};