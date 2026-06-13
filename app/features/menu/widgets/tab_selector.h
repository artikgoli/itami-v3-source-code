#pragma once
#include "base_widget.h"

class c_tab_selector : public c_base_widget
{
public:
	c_tab_selector( const std::string& name, int index, int* menu_var )
	{
		display_name = name;
		internal_index = index;
		menu_index = menu_var;
		width = 70;
		height = 14;
		type = WIDGET_TYPE_TAB_SELECT;
	}

	bool on_input( vector2d pos, vector2d mouse ) override;
	void on_render( vector2d pos ) override;
	void on_render_post( vector2d pos ) override
	{
	};

	int internal_index = 0;
	int* menu_index = nullptr;

	std::vector<std::unique_ptr<c_base_widget>> children {};
};