#pragma once
#include "base_widget.h"

class c_groupbox : public c_base_widget
{
public:
	c_groupbox( );
	void on_render(vector2d pos) override;
	bool on_input(vector2d pos, vector2d mouse) override;
	void on_render_post( vector2d pos ) override;

	float scroll_height = 0;

	std::vector<std::unique_ptr<c_base_widget>> children;
};