#pragma once
#include "base_widget.h"

class c_same_line : public c_base_widget
{
public:
	c_same_line( )
	{
		display_name = "";
		width = 0;
		height = 5;
		type = WIDGET_TYPE_SAME_LINE_SEPERATOR;
		render_enabled = true;
		toggle_function = nullptr;
	}

	bool on_input( vector2d pos, vector2d mouse ) override
	{
		return false;
	};
	void on_render( vector2d pos ) override
	{
	};
	void on_render_post( vector2d pos ) override
	{
	};
};