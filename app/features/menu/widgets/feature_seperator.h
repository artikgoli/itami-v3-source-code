#pragma once
#include "base_widget.h"

class c_colorpicker;

class c_feature_seperator : public c_base_widget
{
public:
	c_feature_seperator( std::string name )
	{
		display_name = name;
		height = 10;
		type = widget_type::WIDGET_TYPE_FEATURE_SEPERATOR;
		width = 140;
		render_enabled = true;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override;

	c_colorpicker* last_open_colorpicker = nullptr;
	
	std::vector<std::unique_ptr<c_base_widget>> children {};
};