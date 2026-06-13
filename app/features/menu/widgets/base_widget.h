#pragma once

enum widget_type : char
{
	WIDGET_TYPE_TAB_SELECT,
	WIDGET_TYPE_FEATURE_SEPERATOR,
	WIDGET_TYPE_GROUPBOX,
	WIDGET_TYPE_BUTTON,
	WIDGET_TYPE_CHECKBOX,
	WIDGET_TYPE_SLIDER,
	WIDGET_TYPE_COMBOBOX,
	WIDGET_TYPE_DYNAMIC_MULTICOMBOBOX,
	WIDGET_TYPE_COLORPICKER,
	WIDGET_TYPE_SAME_LINE_SEPERATOR,
	WIDGET_TYPE_TEXT_INPUT
};

using toggle_function_fn = std::function<bool()>;
using button_function_fn = std::function<void()>;

class c_base_widget
{
public:
	virtual bool on_input( vector2d pos, vector2d mouse ) = 0;
	virtual void on_render( vector2d pos ) = 0;
	virtual void on_render_post( vector2d pos ) = 0;

	toggle_function_fn toggle_function = {};

	std::string display_name {};

	int width = 0;
	int height = 0;

	bool active = false;
	bool hovered = false;

	bool render_enabled = true;
	int unique_id = 0;

	std::unique_ptr<c_base_widget> parent_widget {};
	widget_type type = {};
};