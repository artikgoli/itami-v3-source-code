#include "menu.h"
#include "..\..\renderer\window_handler.h"
#include "..\..\renderer\renderer.h"
#include "..\notifications\notifications.h"
#include "..\misc\nade helper\nade_helper.h"

void c_menu::on_render( )
{
	color bg_color = color( 10, 10, 10, 255 );
	color border_color = config_variables->misc.menu_outline_color->value;

	if ( position.x == 0.f && position.y == 0.f )
	{
		on_window_update( );
	}

	config->discover_configs( );

	int x = position.x;
	int y = position.y;

	render->depth_rect( x, y, size.x, size.y, bg_color, true, border_color, 4 );
	render->rect( x, y, size.x, size.y, color( 0, 0, 0 ), false );

	//{
	//	render->rect(x + 3, y + size.y - 30, size.x - 6, 20 + 7, config_variables->misc.menu_filler_color->value, true);
	//	render->rect(x + 3, y + size.y - 30, size.x - 6, 20 + 7, border_color, false);
	//	render->rect(x + 3 - 1, y + size.y - 30 - 1, size.x - 6 + 2, 20 + 9, color(0,0,0), false );

	//	render->text( x + 8, y + size.y - 30 + 6, draw_pool->font_smooth_big, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true,
	//		"%s | %s", g_security->m_user.m_username );
	//}

	{
		color outline_clr = config_variables->misc.menu_outline_color->value;
		auto extent = draw_pool->font_smooth.text_extent( std::string( "itami.fun" ) );
		render->gradient_line(x + 3, y + extent.y + 6, x + 3 + extent.x + 15, y + extent.y + 6, 
			color( outline_clr.r, outline_clr.g, outline_clr.b, 90 ), outline_clr, color( outline_clr.r, outline_clr.g, outline_clr.b, 90 ), 1.f );
		render->text( x + 8, y + 4, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true,
			"itami.fun" );
	}

	render_tabs( x + size.x, y + 3 );
	render_work_area( x + 10, y + 30 - 14 );
}

void c_menu::render_tabs( int x, int y )
{
	int offset = 0;
	for ( int i = ( int ) tabs.size( ) - 1; i >= 0; i-- )
	{
		auto& tab = tabs[ i ];
		offset -= tab->width + 10;
		tab->on_render( vector2d( x + offset, y ) );
	}
}

void c_menu::render_work_area( int x, int y )
{
	auto& selected_tab = tabs[ this->selected_tab ];
	if ( !selected_tab )
		return;

	int y_offset = 6;

	int child_offset = y_offset + 8;
	int x_child_offset = -2;

	struct post_draw_call
	{
		c_base_widget* widget = {};
		vector2d position = {};
	};

	std::vector<post_draw_call> post_draw_calls = {};

	for ( auto& child : selected_tab->children )
	{
		child->on_render( vector2d( x + x_child_offset, y + child_offset ) );
		post_draw_calls.emplace_back( post_draw_call { child.get( ), vector2d( x + x_child_offset, y + child_offset ) } );
		//child_offset += child->height + 10;
		x_child_offset += child->width + 16;
	}

	for ( auto& p_draw : post_draw_calls )
	{
		p_draw.widget->on_render_post( p_draw.position );
	}
}

void c_menu::handle_work_area_input( int x, int y, UINT msg, vector2d mouse_pos )
{
	auto& selected_tab = tabs[ this->selected_tab ];
	if ( !selected_tab )
		return;

	int y_offset = 6;
	int child_offset = y_offset + 8;
	int x_child_offset = -2;

	for ( auto& child : selected_tab->children )
	{
		child->on_input( vector2d( x + x_child_offset, y + child_offset ), mouse_pos );
		x_child_offset += child->width + 16;
	}
}

void c_menu::handle_tab_input( UINT msg, vector2d mouse_pos )
{
	int x = position.x;
	int y = position.y;

	int offset = 0;
	for ( int i = ( int ) tabs.size( ) - 1; i >= 0; i-- )
	{
		auto& tab = tabs[ i ];
		offset -= tab->width + 10;
		vector2d tab_pos = vector2d( x + offset + size.x, y + 3.f );

		if ( tab->on_input( tab_pos, mouse_pos ) )
			return;
	}
}

void c_menu::on_window_update( )
{
	// recenter so we dont run off to some wierd ass corner
	position = window_handler->window_size * .5f;
}

// we are working with a static mode gui
// every element is built at initialization
// instead of at run-time

void c_menu::add_tab( std::string name )
{
	tabs.emplace_back( std::make_unique<c_tab_selector>( name, ( int ) tabs.size( ), &selected_tab ) );
	work_tab = tabs.back( ).get( );
	global_widget_count++;
}

void c_menu::add_groupbox( )
{
	global_widget_count++;
	work_tab->children.emplace_back( std::make_unique<c_groupbox>( ) );
	work_groupbox = ( c_groupbox* ) work_tab->children.back( ).get( );
	work_tab->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_feature_seperator( std::string name )
{
	global_widget_count++;
	work_groupbox->children.emplace_back( std::make_unique<c_feature_seperator>( name ) );
	work_separator = ( c_feature_seperator* ) work_groupbox->children.back( ).get( );
	work_groupbox->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_groupbox;
	work_groupbox->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_checkbox( std::string name, bool* config_var, c_keybind_variable* keybind, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_checkbox>( name, config_var, keybind, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_slider_float( std::string name, float* config_var, float min_value, float max_value, float step, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_slider_float>( name, config_var, min_value, max_value, step, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_slider_int( std::string name, int* config_var, int min_value, int max_value, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_slider_int>( name, config_var, min_value, max_value, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_colorpicker( std::string name, color* config_var, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_colorpicker>( name, config_var, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_combobox( std::string name, int* config_var, const std::vector<std::string>& items, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_combobox>( name, config_var, items, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_dynamic_combobox( std::string name, int* config_var, std::vector<std::string>* items, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_dynamic_combobox>( name, config_var, items, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_multicombobox( std::string name, const std::vector<c_dynamic_multicombo_item>& items, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_dynamic_multicombo>( name, items, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_button( std::string name, button_function_fn func, toggle_function_fn toggle, int custom_size )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_button>( name, func, toggle, custom_size ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

// todo: toggle_function_fn
void c_menu::same_line( )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_same_line>( ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::add_textbox( std::string name, c_text_variable* var, toggle_function_fn toggle )
{
	global_widget_count++;
	work_separator->children.emplace_back( std::make_unique<c_text_input>( name, var, toggle ) );
	work_separator->children.back( )->parent_widget = ( std::unique_ptr<c_base_widget> ) work_separator;
	work_separator->children.back( )->unique_id = global_widget_count;
}

void c_menu::setup_aimbot_weapon_configs( c_config_variables::c_aimbot::c_weapon_config_base& cfg, int group_idx )
{
	auto toggle = [ = ] ( ) -> bool
		{
			return config_variables->aimbot.selected_weapon_group == group_idx;
		};

	add_slider_int( "fov", &cfg.fov->value, 0, 180, toggle );
	add_checkbox( "smooth", &cfg.smooth->value, nullptr, toggle );
	add_checkbox( "randomize smooth", &cfg.randomized_smooth->value, nullptr, toggle );
	add_slider_float( "randomize x", &cfg.randomized_x->value, 1.f, 10.f, 0.5f, toggle );
	add_slider_float( "randomize y", &cfg.randomized_y->value, 1.f, 10.f, 0.5f, toggle );
	add_combobox( "smooth type", &cfg.smooth_type->value, { "constant", "slow-end", "fast-end" }, toggle );
	add_slider_float( "smooth x", &cfg.smooth_x->value, 1.f, 40.f, 0.5f, toggle );
	add_slider_float( "smooth y", &cfg.smooth_y->value, 1.f, 40.f, 0.5f, toggle );
	add_multicombobox( "hitboxes", {
		{ "head",		&cfg.head->value },
		{ "chest",		&cfg.chest->value },
		{ "pelvis",		&cfg.pelvis->value },
		{ "legs",		&cfg.legs->value },
		{ "toes",		&cfg.toes->value }
		}, toggle );
	add_checkbox( "autowall", &cfg.autowall->value, nullptr, toggle );
	add_slider_int( "minimum damage", &cfg.mindamage->value, 1, 100, toggle );
	add_checkbox( "prediction", &cfg.prediction->value, nullptr, toggle );
}

void c_menu::setup_triggerbot_weapon_configs( c_config_variables::c_triggerbot::c_weapon_config_base& cfg, int group_idx )
{
	auto toggle = [ = ] ( ) -> bool
		{
			return config_variables->triggerbot.selected_weapon_group == group_idx;
		};

	add_checkbox( "delay", &cfg.delay->value, nullptr, toggle );
	add_checkbox( "dynamic delay", &cfg.dynamic_delay->value, nullptr, toggle );
	add_slider_float( "static delay value", &cfg.static_delay_value->value, 0.f, .1f, 0.001f, toggle );
	add_slider_float( "randomize delay", &cfg.dynamic_delay_value->value, 0.f, .05f, 0.001f, toggle );
	add_slider_float( "next attack delay", &cfg.next_attack_delay->value, 0.f, .15f, 0.001f, toggle );
	add_slider_int( "hitchance", &cfg.hitchance->value, 0, 100, toggle );
	add_slider_int( "hitchance accuracy", &cfg.hitchance_accuracy->value, 1, 100, toggle );

	add_multicombobox( "hitboxes", {
		{ "head",		&cfg.head->value },
		{ "chest",		&cfg.chest->value },
		{ "pelvis",		&cfg.pelvis->value },
		{ "legs",		&cfg.legs->value },
		{ "toes",		&cfg.toes->value }
		}, toggle );

	add_checkbox( "autowall", &cfg.autowall->value, nullptr, toggle );
	add_slider_int( "minimum damage", &cfg.mindamage->value, 1, 100, toggle );
}

void c_menu::setup_rcs_weapon_configs( c_config_variables::c_rcs::c_weapon_config_base& cfg, int group_idx )
{
	auto toggle = [ = ] ( ) -> bool
		{
			return config_variables->rcs.selected_weapon_group == group_idx;
		};

	add_checkbox( "enabled", &cfg.enabled->value, nullptr, toggle );
	add_slider_int( "compensate pitch", &cfg.compensate_x->value, 1, 100, toggle );
	add_slider_int( "compensate yaw", &cfg.compensate_y->value, 1, 100, toggle );
	add_slider_int( "smooth pitch", &cfg.smooth_x->value, 1, 20, toggle );
	add_slider_int( "smooth yaw", &cfg.smooth_y->value, 1, 20, toggle );
}

c_menu::c_menu( )
{
	add_tab( "assistance" );
	{
		add_groupbox( );
		{
			add_feature_seperator( "aimbot" );
			{
				add_checkbox( "enabled", &config_variables->aimbot.enabled->value, config_variables->aimbot.aim_key.get( ) );
				add_checkbox( "smoke check", &config_variables->aimbot.smoke_check->value );
				add_checkbox( "fov circle", &config_variables->aimbot.fov_circle->value );
				same_line( );
				add_colorpicker( "", &config_variables->aimbot.fov_circle_color->value );

				add_multicombobox( "global overrides", {
					{ "pistols",		&config_variables->aimbot.pistols.override_global->value },
					{ "heavy pistols",	&config_variables->aimbot.heavy_pistols.override_global->value },
					{ "smgs",			&config_variables->aimbot.smgs.override_global->value },
					{ "rifles",			&config_variables->aimbot.rifles.override_global->value },
					{ "heavy rifles",	&config_variables->aimbot.heavy_rifles.override_global->value },
					{ "snipers",		&config_variables->aimbot.snipers.override_global->value }
					} );

				add_combobox( "weapon group", &config_variables->aimbot.selected_weapon_group,
					{ "default", "pistols", "heavy pistols", "smgs", "rifles", "heavy rifles", "snipers" } );

				setup_aimbot_weapon_configs( config_variables->aimbot.global_weapon, 0 ); // default
				setup_aimbot_weapon_configs( config_variables->aimbot.pistols, 1 ); // pistols
				setup_aimbot_weapon_configs( config_variables->aimbot.heavy_pistols, 2 ); // heavy pistols
				setup_aimbot_weapon_configs( config_variables->aimbot.smgs, 3 ); // smgs
				setup_aimbot_weapon_configs( config_variables->aimbot.rifles, 4 ); // rifles
				setup_aimbot_weapon_configs( config_variables->aimbot.heavy_rifles, 5 ); // heavy rifles
				setup_aimbot_weapon_configs( config_variables->aimbot.snipers, 6 ); // snipers
			}
		}

		add_groupbox( );
		{
			add_feature_seperator( "rcs" );
			{
				add_multicombobox( "global overrides", {
					{ "pistols", &config_variables->rcs.pistols.override_global->value },
					{ "heavy pistols", &config_variables->rcs.heavy_pistols.override_global->value },
					{ "smgs", &config_variables->rcs.smgs.override_global->value },
					{ "rifles", &config_variables->rcs.rifles.override_global->value },
					{ "heavy rifles", &config_variables->rcs.heavy_rifles.override_global->value },
					{ "snipers", &config_variables->rcs.snipers.override_global->value }
					} );

				add_combobox( "weapon group", &config_variables->rcs.selected_weapon_group,
					{ "default", "pistols", "heavy pistols", "smgs", "rifles", "heavy rifles", "snipers" } );

				setup_rcs_weapon_configs( config_variables->rcs.global_weapon, 0 ); // default
				setup_rcs_weapon_configs( config_variables->rcs.pistols, 1 ); // pistols
				setup_rcs_weapon_configs( config_variables->rcs.heavy_pistols, 2 ); // heavy pistols
				setup_rcs_weapon_configs( config_variables->rcs.smgs, 3 ); // smgs
				setup_rcs_weapon_configs( config_variables->rcs.rifles, 4 ); // rifles
				setup_rcs_weapon_configs( config_variables->rcs.heavy_rifles, 5 ); // heavy rifles
				setup_rcs_weapon_configs( config_variables->rcs.snipers, 6 ); // snipers
			}

			add_feature_seperator( "triggerbot" );
			{
				add_checkbox( "enabled", &config_variables->triggerbot.enabled->value, config_variables->triggerbot.trigger_key.get( ) );
				add_checkbox( "smoke check", &config_variables->triggerbot.smoke_check->value );

				add_multicombobox( "global overrides", {
					{ "pistols",		&config_variables->triggerbot.pistols.override_global->value },
					{ "heavy pistols",	&config_variables->triggerbot.heavy_pistols.override_global->value },
					{ "smgs",			&config_variables->triggerbot.smgs.override_global->value },
					{ "rifles",			&config_variables->triggerbot.rifles.override_global->value },
					{ "heavy rifles",	&config_variables->triggerbot.heavy_rifles.override_global->value },
					{ "snipers",		&config_variables->triggerbot.snipers.override_global->value }
					} );

				add_combobox( "weapon group", &config_variables->triggerbot.selected_weapon_group,
					{ "default", "pistols", "heavy pistols", "smgs", "rifles", "heavy rifles", "snipers" } );

				setup_triggerbot_weapon_configs( config_variables->triggerbot.global_weapon, 0 ); // default
				setup_triggerbot_weapon_configs( config_variables->triggerbot.pistols, 1 ); // pistols
				setup_triggerbot_weapon_configs( config_variables->triggerbot.heavy_pistols, 2 ); // heavy pistols
				setup_triggerbot_weapon_configs( config_variables->triggerbot.smgs, 3 ); // smgs
				setup_triggerbot_weapon_configs( config_variables->triggerbot.rifles, 4 ); // rifles
				setup_triggerbot_weapon_configs( config_variables->triggerbot.heavy_rifles, 5 ); // heavy rifles
				setup_triggerbot_weapon_configs( config_variables->triggerbot.snipers, 6 ); // snipers
			}
		}
	}

	add_tab( "esp" );
	{
		add_groupbox( );
		{
			add_feature_seperator( "players" );
			{
				add_checkbox( "enabled", &config_variables->esp.players_enabled->value, config_variables->esp.player_esp_keybind.get( ) );
				add_checkbox( "occluded alpha change", &config_variables->esp.players_invisible_alpha->value );

				static auto occluded_enabled = [ ] ( )
					{
						return config_variables->esp.players_invisible_alpha->value;
					};

				add_slider_float( "alpha", &config_variables->esp.players_invis_alpha_value->value, 0.f, 90.f, 1.f, occluded_enabled );

				add_checkbox( "box", &config_variables->esp.player_box->value );
				same_line( );
				add_colorpicker( "", &config_variables->esp.player_box_color->value );
				add_checkbox( "name", &config_variables->esp.player_name->value );
				add_checkbox( "healthbar", &config_variables->esp.player_healthbar->value );
				add_checkbox( "weapon", &config_variables->esp.player_weapon->value );
				same_line( );
				add_colorpicker( "", &config_variables->esp.player_weapon_clr->value );
				static auto weapon_enabled = [ ] ( )
					{
						return config_variables->esp.player_weapon->value;
					};
				add_checkbox( "ammo bar", &config_variables->esp.player_weapon_ammo->value, nullptr, weapon_enabled );
				same_line( );
				add_colorpicker( "", &config_variables->esp.player_weapon_ammo_clr->value, weapon_enabled );
				add_checkbox( "skeleton", &config_variables->esp.player_skeleton->value );
				same_line( );
				add_colorpicker( "", &config_variables->esp.player_skeleton_color->value );
				add_checkbox( "head hitbox", &config_variables->esp.player_hitbox->value );
				same_line( );
				add_colorpicker( "", &config_variables->esp.player_hitbox_color->value );
				add_multicombobox( "flags", {
					{ "bomb", &config_variables->esp.bomb_flag->value },
					{ "armor", &config_variables->esp.armor_flag->value },
					{ "kit", &config_variables->esp.kit_flag->value },
					{ "zoom", &config_variables->esp.zoom_flag->value },
					{ "flash", &config_variables->esp.flash_flag->value },
					{ "defuse", &config_variables->esp.defuse_flag->value },
					//{ "immune", &config_variables->esp.immune_flag->value }, // todo
					} );

				add_feature_seperator( "indicators" );
				{
					add_checkbox( "out of fov arrows", &config_variables->esp.player_oof->value );
					same_line( );
					add_colorpicker( "", &config_variables->esp.player_oof_color->value );
					add_slider_float( "range", &config_variables->esp.player_oof_range->value, 25.f, 500.f, 5.f, nullptr );
					add_slider_float( "radius", &config_variables->esp.player_oof_radius->value, 70.f, 500.f, 5.f, nullptr );
				add_slider_float( "size", &config_variables->esp.player_oof_size->value, 6.f, 40.f, 1.f, nullptr );
				}

			}
		}
		add_groupbox( );
		{
			add_feature_seperator( "world" );
			{
				add_checkbox( "enabled", &config_variables->esp.world_enabled->value );
				add_checkbox( "projectiles", &config_variables->esp.world_projectiles->value );
				add_checkbox( "grenade timers", &config_variables->esp.world_grenade_timers->value );
				add_checkbox( "bomb", &config_variables->esp.world_bomb->value );
				add_checkbox( "defuse kit", &config_variables->esp.world_kit->value );
			}
		}
	}

	add_tab( "visuals" );
	{
		add_groupbox( );
		{
			add_feature_seperator( "menu" );
			{
				add_colorpicker( "menu outline", &config_variables->misc.menu_outline_color->value );
				add_colorpicker( "menu filler", &config_variables->misc.menu_filler_color->value );
				add_colorpicker( "groupbox outline", &config_variables->misc.groupbox_outline_color->value );
				add_colorpicker( "checkbox fill", &config_variables->misc.checkbox_fill_clr->value );
				add_colorpicker( "slider fill", &config_variables->misc.slider_fill_color->value );
				add_colorpicker( "widget accent", &config_variables->misc.widget_accent_color->value );
				add_checkbox( "solid separator", &config_variables->misc.widget_solid_separator->value );
			}

			add_feature_seperator( "quality of life" );
			{
				add_checkbox( "dark mode flash", &config_variables->misc.dark_mode_flash->value );
				add_checkbox( "screen dim", &config_variables->misc.screen_dim->value );
				add_slider_int( "dim amount", &config_variables->misc.screen_dim_amount->value, 0, 100 );
			}
		}

		add_groupbox( );
		{
			add_feature_seperator( "hit effects" );
			{
				add_checkbox( "hitmarker", &config_variables->misc.hitmarker->value );
				add_checkbox( "hitsound", &config_variables->misc.hitsound->value );
				add_slider_int( "hitsound volume", &config_variables->misc.hitsound_volume->value, 0, 100 );
			}

			add_feature_seperator( "indicators" );
			{
				add_colorpicker( "bomb bar color", &config_variables->misc.bomb_timer_color->value );
				add_checkbox( "keybind list", &config_variables->misc.keybind_list->value );
				add_checkbox( "spectator list", &config_variables->misc.spectator_list->value );
				add_checkbox( "spread indicator", &config_variables->misc.spread_indicator->value );
				same_line( );
				add_colorpicker( "", &config_variables->misc.spread_indicator_color->value );
				add_colorpicker( "dot color", &config_variables->misc.spread_indicator_dot_color->value );
				add_colorpicker( "accurate dot color", &config_variables->misc.spread_indicator_dot_color_success->value );
				add_checkbox( "bomb timer", &config_variables->misc.bomb_timer->value );
			}

			add_feature_seperator( "radar" );
			{
				add_checkbox( "enabled", &config_variables->misc.radar_enabled->value );
				add_slider_int( "size", &config_variables->misc.radar_size->value, 100, 400 );
				add_slider_float( "zoom", &config_variables->misc.radar_zoom->value, 1.f, 4.f, 0.1f );
				add_checkbox( "rotate", &config_variables->misc.radar_rotate->value );
			}
		}
	}

	add_tab( "misc" );
	{
		add_groupbox( );
		{
			add_feature_seperator( "nade helper" );
			{
				add_checkbox( "enabled", &config_variables->misc.nade_helper_enabled->value );
				add_checkbox( "auto aim", &config_variables->misc.nade_helper_auto_aim->value, config_variables->misc.nade_helper_auto_aim_key.get( ) );
				add_slider_int( "max fov", &config_variables->misc.nade_helper_max_fov->value, 1, 90 );
				add_slider_int( "smooth x", &config_variables->misc.nade_helper_smooth_x->value, 1, 30 );
				add_slider_int( "smooth y", &config_variables->misc.nade_helper_smooth_y->value, 1, 30 );

				add_textbox( "spot name", &config_variables->misc.nade_helper_spot_name );
				add_textbox( "throw type", &config_variables->misc.nade_throw_type_input );
				add_button( "add spot", [ ] ( ) { nade_helper->save_spot( ); } );

				same_line( );

				add_button( "delete spot", [ ] ( )
					{
						auto& m = config_variables->misc;
						const int type = m.nade_active_delete_type;
						int fi;
						const std::vector<int>*         indices = nullptr;
						const std::vector<std::string>* names   = nullptr;
						switch ( type )
						{
							case 43: fi = m.selected_flash;      indices = &m.flash_spot_indices;      names = &m.flash_spot_names;      break;
							case 44: fi = m.selected_he;         indices = &m.he_spot_indices;         names = &m.he_spot_names;         break;
							case 45: fi = m.selected_smoke;      indices = &m.smoke_spot_indices;      names = &m.smoke_spot_names;      break;
							case 46: fi = m.selected_molotov;    indices = &m.molotov_spot_indices;    names = &m.molotov_spot_names;    break;
							case 47: fi = m.selected_decoy;      indices = &m.decoy_spot_indices;      names = &m.decoy_spot_names;      break;
							case 48: fi = m.selected_incendiary; indices = &m.incendiary_spot_indices; names = &m.incendiary_spot_names; break;
							default: return;
						}
						if ( !indices || fi < 0 || fi >= ( int ) indices->size( ) )
							return;

						const int entry_idx = ( *indices )[ fi ];

						static int         pending_entry = -1;
						static float       pending_time  = 0.f;
						static std::string pending_name;

						const float now   = global_vars->get_time( );
						const bool  same  = ( pending_entry == entry_idx );
						const bool  fresh = ( now - pending_time ) < 3.f;

						if ( same && fresh )
						{
							nade_helper->remove_entry( entry_idx );
							notif->success( "deleted: " + pending_name );
							pending_entry = -1;
						}
						else
						{
							pending_entry = entry_idx;
							pending_time  = now;
							pending_name  = ( *names )[ fi ];
							notif->info( "click again to delete: " + pending_name );
						}
					} );

				add_button( "export", [ ] ( ) { nade_helper->export_to_clipboard( ); } );
				same_line( );
				add_button( "import", [ ] ( ) { nade_helper->import_from_clipboard( ); } );
			}

			add_feature_seperator( "nade spots" );
			{
				add_dynamic_combobox( "flashes", &config_variables->misc.selected_flash, &config_variables->misc.flash_spot_names );
				add_dynamic_combobox( "explosives", &config_variables->misc.selected_he, &config_variables->misc.he_spot_names );
				add_dynamic_combobox( "smokes", &config_variables->misc.selected_smoke, &config_variables->misc.smoke_spot_names );
				add_dynamic_combobox( "molotovs", &config_variables->misc.selected_molotov, &config_variables->misc.molotov_spot_names );
				add_dynamic_combobox( "decoys", &config_variables->misc.selected_decoy, &config_variables->misc.decoy_spot_names );
				add_dynamic_combobox( "incendiaries", &config_variables->misc.selected_incendiary, &config_variables->misc.incendiary_spot_names );
			}
		}

		add_groupbox( );
		{
			add_feature_seperator( "misc" );
			{
				//same_line( );
				//add_colorpicker( "", &config_variables->misc.bomb_timer_color->value );
				add_button( "unload", [ ] ( )
					{
						PostMessageA( window_handler->window_handle, WM_QUIT, 0, 0 );
					} );
			}

			add_feature_seperator( "configs" );
			{
				add_dynamic_combobox( "configs", &config_variables->selected_config, &config_variables->config_list );
				add_textbox( "config name", &config_variables->misc.config_name_storage );
				// todo add error handling
						// make delete_kfg return true/false depended on success
				add_button( "save", [ ] ( )
					{
						std::replace( config_variables->misc.config_name_storage.value.begin( ),
							config_variables->misc.config_name_storage.value.end( ),
							' ', '_' );

						auto info = notif->begin_info( "saving config" );
						if ( config_variables->config_list.empty( ) )
						{
							if ( config_variables->misc.config_name_storage.value.length( ) <= 0 )
							{
								notif->finish_error( info, "please input a name" );
								return;
							}

							config->save( config_variables->misc.config_name_storage.value.data( ), config_variables->all_variables );
							notif->finish_success( info, ( "saved config: " + config_variables->misc.config_name_storage.value ).c_str( ) );
						}
						else
						{
							int idx = config_variables->selected_config;
							if ( idx < 0 || idx > config_variables->config_list.size( ) )
								notif->finish_error( info, "something went wrong saving config" );

							std::string name = config_variables->config_list[ idx ];
							config->save( config_variables->config_list[ idx ].data( ), config_variables->all_variables );
							notif->finish_success( info, ( "saved config: " + name ).c_str( ) );
						}
					}, nullptr, 56 ); // size is optional // 239 (parent widget width) / 4 - 3

				same_line( );

				add_button( "load", [ ] ( )
					{
						std::replace( config_variables->misc.config_name_storage.value.begin( ),
							config_variables->misc.config_name_storage.value.end( ),
							' ', '_' );

						auto info = notif->begin_info( "loading config" );
						int idx = config_variables->selected_config;
						if ( idx < 0 || idx >= config_variables->config_list.size( ) )
						{
							notif->finish_error( info, "something went wrong loading config" );
							return;
						}

						std::string name = config_variables->config_list[ idx ];
						config->load( config_variables->config_list[ idx ].data( ), config_variables->all_variables );
						notif->finish_success( info, ( "loaded config: " + name ).c_str( ) );
					}, nullptr, 56 );

				same_line( );

				add_button( "create", [ ] ( )
					{
						std::replace( config_variables->misc.config_name_storage.value.begin( ),
							config_variables->misc.config_name_storage.value.end( ),
							' ', '_' );

						auto info = notif->begin_info( "creating config" );
						if ( config_variables->misc.config_name_storage.value.length( ) <= 0 )
						{
							notif->finish_error( info, "please input a name" );
							return;
						}

						int idx = config_variables->selected_config;
						if ( idx < 0 || idx > config_variables->config_list.size( ) )
							notif->finish_error( info, "something went wrong creating config" );

						std::string name = config_variables->misc.config_name_storage.value;
						config->save( config_variables->misc.config_name_storage.value.data( ), config_variables->all_variables );
						notif->finish_success( info, ( "created config: " + name ).c_str( ) );

					}, nullptr, 56 );

				same_line( );

				add_button( "delete", [ ] ( )
					{
						std::replace( config_variables->misc.config_name_storage.value.begin( ),
							config_variables->misc.config_name_storage.value.end( ),
							' ', '_' );

						auto info = notif->begin_info( "deleting config" );
						int idx = config_variables->selected_config;
						if ( idx < 0 || idx >= config_variables->config_list.size( ) )
							notif->finish_error( info, "something went wrong deleting config" );

						std::string name = config_variables->config_list[ idx ];
						config->delete_kfg( config_variables->config_list[ idx ].data( ) );
						notif->finish_success( info, ( "deleted config: " + name ).c_str( ) );

					}, nullptr, 56 );

				add_button( "open folder", [ ] ( )
					{
						ShellExecuteW( nullptr, L"explore",
							( storage_base( ) / "kfg" ).wstring( ).c_str( ),
							nullptr, nullptr, SW_SHOWNORMAL );
					} );
			}
		}
	}
}