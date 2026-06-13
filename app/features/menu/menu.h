#pragma once
#include "input/input.h"
#include "../c_base_feature.h"
#include "widgets\base_widget.h"
#include "widgets\tab_selector.h"
#include "widgets\feature_seperator.h"
#include "widgets\groupbox.h"
#include "widgets\checkbox.h"
#include "widgets\slider_float.h"
#include "widgets\colorpicker.h"
#include "widgets\combobox.h"
#include "widgets\slider_int.h"
#include "widgets\dynamic_multicombo.h"
#include "widgets\button.h"
#include "widgets\same_line.h"
#include "widgets\text_input.h"
#include "widgets\dynamic_combobox.h"

enum e_menu_tabs : int
{
	TAB_AIMBOT = 0,
	TAB_VISUALS,
	TAB_MISC,
	TAB_SETTINGS
};

class c_menu : public c_base_feature
{
public:
	c_menu( );

	void on_render( ) override;
	void render_tabs( int x, int y );
	void render_work_area( int x, int y );
	void handle_work_area_input( int x, int y, UINT msg, vector2d mouse_pos );
	void on_entity_update( const float& t ) override
	{
	};

	void handle_tab_input( UINT msg, vector2d mouse_pos );
	void on_window_update( );

	std::vector<std::unique_ptr<c_tab_selector>> tabs {};
	int selected_config = 0;
	std::vector<std::string> configs {};

	int global_widget_count = 0;
	int input_block_id = 0;

	vector2d size = { 530, 500 };
	int selected_tab = e_menu_tabs::TAB_AIMBOT;

	bool is_open = false;
	vector2d position = { 100, 100 };

private:

	void add_tab( std::string name );
	void add_groupbox( );
	void add_feature_seperator( std::string name );
	void add_checkbox(std::string name, bool* config_var, c_keybind_variable* keybind = nullptr, toggle_function_fn toggle = nullptr);
	void add_slider_float(std::string name, float* config_var, float min_value, float max_value, float step, toggle_function_fn toggle = nullptr);
	void add_slider_int(std::string name, int* config_var, int min_value, int max_value, toggle_function_fn toggle = nullptr);
	void add_colorpicker(std::string name, color* config_var, toggle_function_fn toggle = nullptr);
	void add_combobox(std::string name, int* config_var, const std::vector<std::string>& items, toggle_function_fn toggle = nullptr);
	void add_dynamic_combobox( std::string name, int* config_var, std::vector<std::string>* items, toggle_function_fn toggle = nullptr );
	void add_multicombobox(std::string name, const std::vector<c_dynamic_multicombo_item>& items, toggle_function_fn toggle = nullptr);
	void add_button( std::string name, button_function_fn func, toggle_function_fn toggle = nullptr, int custom_size = -1 );
	void same_line( );
	void add_textbox( std::string name, c_text_variable* var, toggle_function_fn toggle = nullptr );

	void setup_aimbot_weapon_configs( c_config_variables::c_aimbot::c_weapon_config_base& cfg, int group_idx );
	void setup_triggerbot_weapon_configs( c_config_variables::c_triggerbot::c_weapon_config_base& cfg, int group_idx );
	void setup_rcs_weapon_configs( c_config_variables::c_rcs::c_weapon_config_base& cfg, int group_idx );

	c_groupbox* work_groupbox = {};
	c_tab_selector* work_tab = {};
	c_feature_seperator* work_separator {};
};

inline std::unique_ptr<c_menu> menu = nullptr;