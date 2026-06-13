#pragma once
#include <filesystem>

inline std::filesystem::path storage_base( )
{
	wchar_t* p = nullptr;
	if ( SUCCEEDED( SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, nullptr, &p ) ) )
	{
		std::filesystem::path result( p );
		CoTaskMemFree( p );
		return result / "cs2";
	}
	return { };
}

enum class config_var_type : uint8_t
{
	boolean,
	integer,
	floating,
	color,
	vec2d,
	keybind,
	text
};

struct c_base_config_variable
{
	c_base_config_variable( ) = default;

	virtual void save( std::ofstream& out ) const = 0;
	virtual config_var_type get_type( ) const = 0;
	virtual void load( const char* value ) = 0;

	char variable_name[ 256 ] = "\0";
	c_base_config_variable* parent_variable = nullptr;
};

struct c_text_variable : c_base_config_variable
{
	std::string value;
	bool is_typing = false;
	size_t max_length = 128;

	config_var_type get_type( ) const override
	{
		return config_var_type::text;
	}
	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = \"" << value << "\"\n";
	}
	void load( const char* val ) override
	{
		std::string s = val;

		size_t first = s.find_first_of( '\"' );
		size_t last = s.find_last_of( '\"' );
		if ( first != std::string::npos && last != std::string::npos && first < last ) {
			value = s.substr( first + 1, last - first - 1 );
		}
		else {
			value = s;
		}
	}
	c_text_variable( ) = default;
	c_text_variable( std::string txt )
	{
	}
};

struct c_keybind_variable : c_base_config_variable
{
	int key = 0;
	int activation_type = 0;

	int previous_key = 0;
	bool waiting_for_input = false;
	bool additional_options_open = false;

	bool active = false;
	char display_name[ 64 ] = {};

	config_var_type get_type( ) const override
	{
		return config_var_type::keybind;
	}
	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = " << key << " " << activation_type << '\n';
	}
	void load( const char* val ) override;
	c_keybind_variable( ) = default;
	c_keybind_variable( int k )
		: key( k )
	{
		activation_type = 1;
	}

	c_keybind_variable( int k, int act_type )
		: key( k )
	{
		activation_type = act_type;
	}
};

struct c_bool_variable : c_base_config_variable
{
	bool value = false;

	bool is_enabled( ) const
	{
		// todo: keybind

		return value;
	}

	config_var_type get_type( ) const override
	{
		return config_var_type::boolean;
	}

	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = " << ( value ? 1 : 0 ) << '\n';
	}

	void load( const char* val ) override
	{
		value = std::atoi( val ) != 0;
	}

	c_bool_variable( ) = default;
	c_bool_variable( bool val )
		: value( val )
	{
	}
};

struct c_float_variable : c_base_config_variable
{
	float value = 0.f;
	config_var_type get_type( ) const override
	{
		return config_var_type::floating;
	}
	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = " << value << '\n';
	}
	void load( const char* val ) override
	{
		value = std::stof( val );
	}
	c_float_variable( ) = default;
	c_float_variable( float val )
		: value( val )
	{
	}
};

struct c_int_variable : c_base_config_variable
{
	int value = 0;
	config_var_type get_type( ) const override
	{
		return config_var_type::integer;
	}
	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = " << value << '\n';
	}
	void load( const char* val ) override
	{
		value = std::atoi( val );
	}
	c_int_variable( ) = default;
	c_int_variable( int val )
		: value( val )
	{
	}
};

struct c_color_variable : c_base_config_variable
{
	color value = color( 255, 255, 255, 255 );
	config_var_type get_type( ) const override
	{
		return config_var_type::color;
	}
	void save( std::ofstream& out ) const override
	{
		out << variable_name << " = " << ( int ) value.r << ' ' <<
			( int ) value.g << ' ' << ( int ) value.b << ' ' << ( int ) value.a << '\n';
	}
	void load( const char* val ) override
	{
		int r, g, b, a;
		sscanf_s( val, "%d %d %d %d", &r, &g, &b, &a );
		value = color( ( uint8_t ) r, ( uint8_t ) g, ( uint8_t ) b, ( uint8_t ) a );
	}
	c_color_variable( ) = default;
	c_color_variable( color val )
		: value( val )
	{
	}
};

class c_config
{
public:
	void save( const char* path, const std::vector<c_base_config_variable*>& vars );
	void delete_kfg( const char* path );
	void load( const char* path, std::vector<c_base_config_variable*>& vars );
	void discover_configs( );
};


class c_config_variables
{
public:
	c_config_variables( );

	/*
		REMEMBER
		YOU NEED TO CREATE THE VARIABLE IN CONFIG.CPP
	*/

	//std::unique_ptr<c_color_variable> accent_color {};

	struct c_aimbot
	{
		std::unique_ptr<c_bool_variable> enabled {};
		std::unique_ptr<c_keybind_variable> aim_key {};
		std::unique_ptr<c_bool_variable> fov_circle {};
		std::unique_ptr<c_color_variable> fov_circle_color {};
		std::unique_ptr<c_bool_variable> smoke_check {};

		struct c_weapon_config_base
		{
			std::unique_ptr<c_bool_variable> override_global {};
			std::unique_ptr<c_int_variable> fov {};
			std::unique_ptr<c_bool_variable> smooth {};
			std::unique_ptr<c_bool_variable> randomized_smooth {};
			std::unique_ptr<c_float_variable> randomized_x {};
			std::unique_ptr<c_float_variable> randomized_y {};
			std::unique_ptr<c_int_variable> smooth_type {};
			std::unique_ptr<c_float_variable> smooth_x {};
			std::unique_ptr<c_float_variable> smooth_y {};
			std::unique_ptr<c_bool_variable> autowall {};
			std::unique_ptr<c_int_variable> mindamage {};

			std::unique_ptr<c_bool_variable> head {};
			std::unique_ptr<c_bool_variable> chest {};
			std::unique_ptr<c_bool_variable> pelvis {};
			std::unique_ptr<c_bool_variable> legs {};
			std::unique_ptr<c_bool_variable> toes {};

			std::unique_ptr<c_bool_variable> prediction {};

		} global_weapon;

		c_weapon_config_base pistols = {};
		c_weapon_config_base heavy_pistols = {};
		c_weapon_config_base rifles = {};
		c_weapon_config_base heavy_rifles = {};
		c_weapon_config_base smgs = {};
		c_weapon_config_base snipers = {};

		int selected_weapon_group = {};
		
	} aimbot;

	struct c_triggerbot
	{
		std::unique_ptr<c_bool_variable> enabled {};
		std::unique_ptr<c_keybind_variable> trigger_key {};
		std::unique_ptr<c_bool_variable> smoke_check {};

		struct c_weapon_config_base
		{
			std::unique_ptr<c_bool_variable> override_global {};
			std::unique_ptr<c_bool_variable> head {};
			std::unique_ptr<c_bool_variable> chest {};
			std::unique_ptr<c_bool_variable> pelvis {};
			std::unique_ptr<c_bool_variable> legs {};
			std::unique_ptr<c_bool_variable> toes {};

			std::unique_ptr<c_bool_variable> delay {};
			std::unique_ptr<c_bool_variable> dynamic_delay {};

			std::unique_ptr<c_float_variable> static_delay_value {};
			std::unique_ptr<c_float_variable> dynamic_delay_value {};
			std::unique_ptr<c_float_variable> next_attack_delay {};

			std::unique_ptr<c_int_variable> hitchance {};
			std::unique_ptr<c_int_variable> hitchance_accuracy {};

			std::unique_ptr<c_bool_variable> autowall {};
			std::unique_ptr<c_int_variable> mindamage {};

		} global_weapon;

		c_weapon_config_base pistols = {};
		c_weapon_config_base heavy_pistols = {};
		c_weapon_config_base rifles = {};
		c_weapon_config_base heavy_rifles = {};
		c_weapon_config_base smgs = {};
		c_weapon_config_base snipers = {};

		int selected_weapon_group = {};

	} triggerbot;

	struct c_rcs
	{
		struct c_weapon_config_base
		{
			std::unique_ptr<c_bool_variable> override_global {};
			std::unique_ptr<c_bool_variable> enabled {};
			std::unique_ptr<c_int_variable> compensate_x {};
			std::unique_ptr<c_int_variable> compensate_y {};
			std::unique_ptr<c_int_variable> smooth_x {};
			std::unique_ptr<c_int_variable> smooth_y {};

		} global_weapon;

		c_weapon_config_base pistols = {};
		c_weapon_config_base heavy_pistols = {};
		c_weapon_config_base rifles = {};
		c_weapon_config_base heavy_rifles = {};
		c_weapon_config_base smgs = {};
		c_weapon_config_base snipers = {};

		int selected_weapon_group = {};
	} rcs;

	struct c_misc
	{
		std::unique_ptr<c_bool_variable> hitmarker {};
		std::unique_ptr<c_bool_variable> hitsound {};
		std::unique_ptr<c_int_variable> hitsound_volume {};
		std::unique_ptr<c_bool_variable> keybind_list {};
		std::unique_ptr<c_bool_variable> dark_mode_flash {};
		std::unique_ptr<c_bool_variable> screen_dim {};
		std::unique_ptr<c_int_variable> screen_dim_amount {};
		std::unique_ptr<c_bool_variable> spectator_list {};
		std::unique_ptr<c_float_variable> spectator_list_x {};
		std::unique_ptr<c_float_variable> spectator_list_y {};
		std::unique_ptr<c_bool_variable> spread_indicator {};
		std::unique_ptr<c_color_variable> spread_indicator_color {};
		std::unique_ptr<c_color_variable> spread_indicator_dot_color {};
		std::unique_ptr<c_color_variable> spread_indicator_dot_color_success {};
		std::unique_ptr<c_float_variable> keybind_list_x {};
		std::unique_ptr<c_float_variable> keybind_list_y {};

		c_text_variable config_name_storage = {};

		// todo: struct c_menu_colors
		std::unique_ptr<c_color_variable> menu_outline_color {};
		std::unique_ptr<c_color_variable> menu_filler_color {};
		std::unique_ptr<c_color_variable> groupbox_outline_color {};
		std::unique_ptr<c_color_variable> checkbox_fill_clr {};
		std::unique_ptr<c_color_variable> slider_fill_color {};
		std::unique_ptr<c_color_variable> widget_accent_color {};
		std::unique_ptr<c_bool_variable> widget_solid_separator {};

		int nade_helper_selected_map = 0;

		std::unique_ptr<c_bool_variable> bomb_timer {};
		std::unique_ptr<c_float_variable> bomb_timer_x {};
		std::unique_ptr<c_float_variable> bomb_timer_y {};
		std::unique_ptr<c_color_variable> bomb_timer_color {};

		std::unique_ptr<c_bool_variable>  radar_enabled {};
		std::unique_ptr<c_float_variable> radar_x {};
		std::unique_ptr<c_float_variable> radar_y {};
		std::unique_ptr<c_int_variable>   radar_size {};
		std::unique_ptr<c_float_variable> radar_zoom {};
		std::unique_ptr<c_bool_variable>  radar_rotate {};

		std::unique_ptr<c_bool_variable> nade_helper_enabled {};
		std::unique_ptr<c_bool_variable> nade_helper_auto_aim {};
		std::unique_ptr<c_keybind_variable> nade_helper_auto_aim_key {};
		std::unique_ptr<c_int_variable> nade_helper_max_fov {};
		std::unique_ptr<c_int_variable> nade_helper_smooth_x {};
		std::unique_ptr<c_int_variable> nade_helper_smooth_y {};

		// nade helper ui state (not saved to config)
		int selected_spot = 0;
		std::vector<std::string> spot_names {};
		c_text_variable nade_helper_spot_name {};
		c_text_variable nade_throw_type_input {};

		// per-type spot lists (43=flash, 44=he, 45=smoke, 46=molotov, 47=decoy, 48=incendiary)
		int selected_flash       = 0;
		int selected_he          = 0;
		int selected_smoke       = 0;
		int selected_molotov     = 0;
		int selected_decoy       = 0;
		int selected_incendiary  = 0;
		std::vector<std::string> flash_spot_names {};
		std::vector<std::string> he_spot_names {};
		std::vector<std::string> smoke_spot_names {};
		std::vector<std::string> molotov_spot_names {};
		std::vector<std::string> decoy_spot_names {};
		std::vector<std::string> incendiary_spot_names {};
		std::vector<int>         flash_spot_indices {};
		std::vector<int>         he_spot_indices {};
		std::vector<int>         smoke_spot_indices {};
		std::vector<int>         molotov_spot_indices {};
		std::vector<int>         decoy_spot_indices {};
		std::vector<int>         incendiary_spot_indices {};
		int nade_active_delete_type = 45; // which list delete operates on
	} misc;

	struct c_esp
	{
		std::unique_ptr<c_bool_variable> players_enabled {};
		std::unique_ptr<c_keybind_variable> player_esp_keybind {};
		std::unique_ptr<c_bool_variable> players_invisible_alpha {};
		std::unique_ptr<c_float_variable> players_invis_alpha_value {};
		std::unique_ptr<c_bool_variable> player_box {};
		std::unique_ptr<c_color_variable> player_box_color {};
		std::unique_ptr<c_bool_variable> player_name {};
		std::unique_ptr<c_bool_variable> player_healthbar {};
		std::unique_ptr<c_bool_variable> player_weapon {};
		std::unique_ptr<c_color_variable> player_weapon_clr {};
		std::unique_ptr<c_bool_variable> player_weapon_ammo {};
		std::unique_ptr<c_color_variable> player_weapon_ammo_clr {};
		std::unique_ptr<c_bool_variable> player_oof {};
		std::unique_ptr<c_color_variable> player_oof_color {};
		std::unique_ptr<c_float_variable> player_oof_range {};
		std::unique_ptr<c_float_variable> player_oof_radius {};
		std::unique_ptr<c_float_variable> player_oof_size {};
		std::unique_ptr<c_bool_variable> player_skeleton {};
		std::unique_ptr<c_color_variable> player_skeleton_color {};
		std::unique_ptr<c_bool_variable> player_hitbox {};
		std::unique_ptr<c_color_variable> player_hitbox_color {};

		std::unique_ptr<c_bool_variable> bomb_flag {};
		std::unique_ptr<c_bool_variable> armor_flag {};
		std::unique_ptr<c_bool_variable> kit_flag {};
		std::unique_ptr<c_bool_variable> zoom_flag {};
		std::unique_ptr<c_bool_variable> flash_flag {};
		std::unique_ptr<c_bool_variable> defuse_flag {};
		std::unique_ptr<c_bool_variable> immune_flag {};

		std::unique_ptr<c_bool_variable> world_enabled {};
		std::unique_ptr<c_bool_variable> world_projectiles {};
		std::unique_ptr<c_bool_variable> world_bomb {};
		std::unique_ptr<c_bool_variable> world_kit {};
		std::unique_ptr<c_bool_variable> world_grenade_timers {};
	} esp;


	std::vector<std::string> config_list = {};
	int selected_config = 0;

	// do not remove
	std::vector<c_base_config_variable*> all_variables = {};
};

inline auto config_variables = std::make_shared<c_config_variables>( );
inline auto config = std::make_unique<c_config>( );