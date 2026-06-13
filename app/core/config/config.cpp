#include "config.h"
#include "..\..\features\menu\input\input.h"

template<typename T, typename... Args>
T* create_variable_internal(
	const char* name,
	std::unique_ptr<T>& storage,
	std::vector<c_base_config_variable*>& container, Args&&... args )
{
	static_assert( std::is_base_of<c_base_config_variable, T>::value,
		"config variable type must inherit from c_base_config_variable" );

	static_assert(
		std::is_base_of<c_base_config_variable, T>::value,
		"config variable type must inherit from c_base_config_variable"
		);

	storage = std::make_unique<T>( std::forward<Args>( args )... );

	strncpy_s(
		storage->variable_name,
		sizeof( storage->variable_name ),
		name,
		_TRUNCATE
	);

	container.emplace_back( storage.get( ) );
	return storage.get( );
}

// for keybinds you can pass int NAME, KEY, (optional) ACTIVATION_TYPE
#define create_variable( var, ... ) \
	create_variable_internal( #var, var, all_variables, __VA_ARGS__ )

// define nesting
// mmmm finger licking good
#define CREATE_AIMBOT_GROUP(name)                           \
	create_variable( aimbot.name.override_global, false );  \
	create_variable( aimbot.name.fov, 50 );                 \
	create_variable( aimbot.name.smooth, true );            \
	create_variable( aimbot.name.randomized_smooth, true );            \
	create_variable( aimbot.name.randomized_x, 3.f );            \
	create_variable( aimbot.name.randomized_y, 3.f );            \
	create_variable( aimbot.name.smooth_type, 0 );          \
	create_variable( aimbot.name.smooth_x, 1.f );           \
	create_variable( aimbot.name.smooth_y, 1.f );           \
	create_variable( aimbot.name.autowall, false );          \
	create_variable( aimbot.name.mindamage, 20.f );          \
	create_variable( aimbot.name.prediction, false ); \
	create_variable( aimbot.name.head, true ); \
	create_variable( aimbot.name.chest, true ); \
	create_variable( aimbot.name.pelvis, true ); \
	create_variable( aimbot.name.legs, true ); \
	create_variable( aimbot.name.toes, true );

#define CREATE_TRIGGERBOT_GROUP(name)                              \
	create_variable( triggerbot.name.override_global, false );     \
	create_variable( triggerbot.name.delay, true );                \
	create_variable( triggerbot.name.dynamic_delay, true );        \
	create_variable( triggerbot.name.static_delay_value, 0.2f );   \
	create_variable( triggerbot.name.dynamic_delay_value, 0.2f );  \
	create_variable( triggerbot.name.next_attack_delay, 0.03f );  \
	create_variable( triggerbot.name.hitchance, 50 );              \
	create_variable( triggerbot.name.hitchance_accuracy, 50 );	   \
	create_variable( triggerbot.name.autowall, false );			   \
	create_variable( triggerbot.name.mindamage, false );		   \
	create_variable( triggerbot.name.head, true ); \
	create_variable( triggerbot.name.chest, true ); \
	create_variable( triggerbot.name.pelvis, true ); \
	create_variable( triggerbot.name.legs, true ); \
	create_variable( triggerbot.name.toes, true );


#define CREATE_RCS_GROUP(name)									   \
	create_variable( rcs.name.override_global, false );			   \
	create_variable( rcs.name.enabled, true );					   \
	create_variable( rcs.name.compensate_x, 80 );				   \
	create_variable( rcs.name.compensate_y, 80 );				   \
	create_variable( rcs.name.smooth_x, 3 );					   \
	create_variable( rcs.name.smooth_y, 3 );					   \

// add config vars here ALSO
// you NEED to create them
c_config_variables::c_config_variables( )
{
	create_variable( aimbot.enabled, true );
	{
		create_variable( aimbot.aim_key, 0 );
		create_variable( aimbot.fov_circle, true );
		create_variable( aimbot.fov_circle_color, color( 255, 255, 255, 255 ) );
		create_variable( aimbot.smoke_check, false );

		// :exploding_head:

		CREATE_AIMBOT_GROUP( global_weapon );
		CREATE_AIMBOT_GROUP( pistols );
		CREATE_AIMBOT_GROUP( heavy_pistols );
		CREATE_AIMBOT_GROUP( smgs );
		CREATE_AIMBOT_GROUP( rifles );
		CREATE_AIMBOT_GROUP( heavy_rifles );
		CREATE_AIMBOT_GROUP( snipers );
	}

	CREATE_RCS_GROUP( global_weapon );
	CREATE_RCS_GROUP( pistols );
	CREATE_RCS_GROUP( heavy_pistols );
	CREATE_RCS_GROUP( smgs );
	CREATE_RCS_GROUP( rifles );
	CREATE_RCS_GROUP( heavy_rifles );
	CREATE_RCS_GROUP( snipers );

	create_variable( triggerbot.enabled, true );
	{
		create_variable( triggerbot.trigger_key, 0 );
		create_variable( triggerbot.smoke_check, false );
		CREATE_TRIGGERBOT_GROUP( global_weapon );
		CREATE_TRIGGERBOT_GROUP( pistols );
		CREATE_TRIGGERBOT_GROUP( heavy_pistols );
		CREATE_TRIGGERBOT_GROUP( smgs );
		CREATE_TRIGGERBOT_GROUP( rifles );
		CREATE_TRIGGERBOT_GROUP( heavy_rifles );
		CREATE_TRIGGERBOT_GROUP( snipers );
	}

	create_variable( esp.players_enabled, true );
	{
		create_variable( esp.player_esp_keybind, 0, 0 );
		create_variable( esp.players_invisible_alpha, false );
		create_variable( esp.players_invis_alpha_value, 30.f );
		create_variable( esp.player_box, true );
		create_variable( esp.player_box_color, color( 255, 0, 0 ) );
		create_variable( esp.player_name, true );
		create_variable( esp.player_healthbar, true );
		create_variable( esp.player_weapon, true );
		create_variable( esp.player_weapon_clr, color( 255, 255, 255 ) );
		create_variable( esp.player_weapon_ammo, true );
		create_variable( esp.player_weapon_ammo_clr, color( 200, 200, 200 ) );
		create_variable( esp.player_oof, true );
		create_variable( esp.player_oof_color, color( 255, 0, 0, 140 ) );
		create_variable( esp.player_oof_range, 100.f );
		create_variable( esp.player_oof_radius, 340.f );
		create_variable( esp.player_oof_size, 28.f );
		create_variable( esp.player_skeleton, true );
		create_variable( esp.player_skeleton_color, color( 255, 255, 255, 255 ) );
		create_variable( esp.player_hitbox, false );
		create_variable( esp.player_hitbox_color, color( 255, 80, 80, 120 ) );
		create_variable( esp.bomb_flag, true );
		create_variable( esp.armor_flag, true );
		create_variable( esp.kit_flag, true );
		create_variable( esp.flash_flag, true );
		create_variable( esp.zoom_flag, true );
		create_variable( esp.defuse_flag, true );
		create_variable( esp.immune_flag, true );
	}

	create_variable( esp.world_enabled, true );
	{
		create_variable( esp.world_projectiles, true );
		create_variable( esp.world_bomb, true );
		create_variable( esp.world_kit, true );
		create_variable( esp.world_grenade_timers, true );
	}

	create_variable( misc.hitmarker, true );
	create_variable( misc.dark_mode_flash, false );
	create_variable( misc.screen_dim, false );
	create_variable( misc.screen_dim_amount, 50 );
	create_variable( misc.spectator_list, true );
	create_variable( misc.spectator_list_x, 20.f );
	create_variable( misc.spectator_list_y, 20.f );
	create_variable( misc.spread_indicator, true );
	create_variable( misc.spread_indicator_color, color( 255, 255, 255, 40 ) );
	create_variable( misc.spread_indicator_dot_color, color( 255, 80, 80, 220 ) );
	create_variable( misc.spread_indicator_dot_color_success, color( 80, 255, 80, 220 ) );
	create_variable( misc.keybind_list, true );
	create_variable( misc.keybind_list_x, 50.f );
	create_variable( misc.keybind_list_y, 200.f );
	create_variable( misc.menu_outline_color, color( 130, 40, 225, 240 ) );
	create_variable( misc.menu_filler_color, color( 130, 40, 225, 40 ) );
	create_variable( misc.groupbox_outline_color, color( 130, 40, 225, 240 ) );
	create_variable( misc.checkbox_fill_clr, color( 210, 120, 255, 225 ) );
	create_variable( misc.slider_fill_color, color( 170, 110, 255, 240 ) );
	create_variable( misc.widget_accent_color, color( 130, 40, 225, 240 ) );
	create_variable( misc.widget_solid_separator, false );

	create_variable(misc.hitsound, true);
	{
		create_variable( misc.hitsound_volume, 50 );
	}

	create_variable( misc.bomb_timer, true );
	create_variable( misc.bomb_timer_x, 20.f );
	create_variable( misc.bomb_timer_y, 60.f );
	create_variable( misc.bomb_timer_color, color( 255, 80, 80, 220 ) );

	create_variable( misc.radar_enabled, true );
	create_variable( misc.radar_x, 20.f );
	create_variable( misc.radar_y, 20.f );
	create_variable( misc.radar_size, 200 );
	create_variable( misc.radar_zoom, 1.f );
	create_variable( misc.radar_rotate, true );

	create_variable( misc.nade_helper_enabled, false );
	create_variable( misc.nade_helper_auto_aim, true );
	create_variable( misc.nade_helper_auto_aim_key, 0, 1 );
	strcpy_s( misc.nade_helper_auto_aim_key->display_name, "auto-aim" );
	create_variable( misc.nade_helper_max_fov, 50 );
	create_variable( misc.nade_helper_smooth_x, 5 );
	create_variable( misc.nade_helper_smooth_y, 5 );

	// DO NOT REMOVE WARNING WARNING DO NOT TOUCH EVEN
	misc.config_name_storage = {};
	this->all_variables.push_back( &misc.config_name_storage );
	// END

	misc.nade_helper_spot_name = {};
	this->all_variables.push_back( &misc.nade_helper_spot_name );

	misc.nade_throw_type_input = {};
	this->all_variables.push_back( &misc.nade_throw_type_input );

}

void c_config::save( const char* path, const std::vector<c_base_config_variable*>& vars )
{
	try
	{
		auto p = storage_base( ) / "kfg";

		if ( !std::filesystem::exists( p ) )
			std::filesystem::create_directories( p );

		std::ofstream out( p / path, std::ios::trunc );

		if ( !out.is_open( ) )
		{
			MessageBoxA( 0, "failed to open kfg (save)", "kfg err0r 1337", MB_OK | MB_ICONERROR );
			return;
		}

		out.exceptions( std::ofstream::failbit | std::ofstream::badbit );

		for ( const auto* var : vars )
			var->save( out );

		out.close( );
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( 0, e.what( ), "kfg save exception", MB_OK | MB_ICONERROR );
	}
}


void c_config::delete_kfg( const char* path )
{
	try
	{
		auto full = storage_base( ) / "kfg" / path;

		if ( !std::filesystem::exists( full ) )
		{
			MessageBoxA( 0, "config does not exist (delete)", "kfg err0r 1337", MB_OK | MB_ICONERROR );
			return;
		}

		if ( !std::filesystem::remove( full ) )
		{
			MessageBoxA( 0, "failed to remove config (delete)", "kfg err0r 1337", MB_OK | MB_ICONERROR );
			return;
		}
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( 0, e.what( ), "kfg delete exception", MB_OK | MB_ICONERROR );
	}
}


void c_config::load( const char* path, std::vector<c_base_config_variable*>& vars )
{
	try
	{
		auto full = storage_base( ) / "kfg" / path;

		std::ifstream in( full );

		if (!in.is_open( ))
		{
			MessageBoxA( 0, "failed to open kfg (load)", "kfg err0r 1337", MB_OK | MB_ICONERROR );
			return;
		}

		std::string line;

		while ( std::getline( in, line ) )
		{
			auto pos = line.find( '=' );
			if ( pos == std::string::npos )
				continue;

			std::string key = line.substr( 0, pos );
			std::string value = line.substr( pos + 1 );

			key.erase( key.find_last_not_of( " \t" ) + 1 );
			value.erase( 0, value.find_first_not_of( " \t" ) );

			for ( auto* var : vars )
			{
				if ( std::strcmp( var->variable_name, key.c_str( ) ) == 0)
				{
					var->load( value.c_str( ) );
					break;
				}
			}
		}
	}
	catch ( const std::exception& e )
	{
		MessageBoxA( 0, e.what( ), "kfg load exception", MB_OK | MB_ICONERROR );
	}
}

void c_config::discover_configs( )
{
	auto p = storage_base( ) / "kfg";
	config_variables->config_list.clear( );

	try
	{
		if ( !std::filesystem::exists( p ) )
			std::filesystem::create_directory( p );

		if ( std::filesystem::is_directory( p ) )
		{
			for ( const auto& entry : std::filesystem::directory_iterator( p ) )
			{
				if ( entry.is_regular_file( ) )
				{
					std::string config_name = entry.path( ).filename( ).string( );
					config_variables->config_list.emplace_back( config_name );
				}
			}
		}
	}
	catch ( const std::filesystem::filesystem_error& e )
	{
		MessageBoxA( 0, e.what( ), "kfg discover exception", MB_OK | MB_ICONERROR );
	}
}

void c_keybind_variable::load( const char* val )
{
	int k = 0, t = 0;
	if ( sscanf_s( val, "%d %d", &k, &t ) >= 1 )
	{
		this->key = k;
		this->activation_type = t;
	}

	if ( this->previous_key != 0 )
		input_handler->remove_keybind( this->previous_key );

	input_handler->add_keybind( this->key ); // yeah thats needed bro
}
