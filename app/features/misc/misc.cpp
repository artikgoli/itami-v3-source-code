#include "misc.h"
#include "..\menu\menu.h"
#include "..\menu\input\input.h"
#include "..\..\renderer\renderer.h"
#include "..\..\renderer\window_handler.h"
#include "..\entity_cache\entity_cache.h"
#include "..\..\..\valve\trace\trace.h"
#include <numbers>
#include "..\esp\esp.h"
#include "..\..\core\vpk\vpk_radar.h"

static void load_radar_for_map( c_misc* m, std::string map_name )
{
	const auto cs2_dir = get_cs2_game_dir( );
	if ( cs2_dir.empty( ) )
	{
		log_dbg( "radar: cs2 dir not found" );
		return;
	}

	const auto vpk_dir = cs2_dir / "game" / "csgo";

	std::string found_name;
	auto vtex_bytes = vpk_read_overhead_map( vpk_dir, map_name, &found_name );
	if ( vtex_bytes.empty( ) )
	{
		log_dbg( "radar: no overheadmap found for '%s'", map_name.c_str( ) );
		return;
	}

	vtex_result vr;
	if ( !vtex_parse( vtex_bytes, vr ) )
	{
		log_dbg( "radar: vtex_parse failed for '%s'", found_name.c_str( ) );
		return;
	}

	c_misc::c_radar_transform transform {};
	auto ov_bytes = vpk_read_file( vpk_dir, "resource/overviews/" + map_name + ".txt" );
	if ( !ov_bytes.empty( ) )
	{
		vpk_parse_overview( ov_bytes, transform.x, transform.y, transform.scale );
		if ( transform.scale <= 0.f )
			transform.scale = 5.f;
	}
	else
	{
		transform.scale = 5.f;
		log_dbg( "radar: no overview for '%s', scale defaulted", map_name.c_str( ) );
	}

	log_dbg( "radar: %dx%d fmt=%u  x=%.0f y=%.0f scale=%.2f",
	         vr.width, vr.height, (unsigned)vr.dxgi_fmt,
	         transform.x, transform.y, transform.scale );

	{
		std::lock_guard lock( m->radar_mutex );
		m->radar_pending_rgba      = std::move( vr.mip0_data );
		m->radar_pending_w         = vr.width;
		m->radar_pending_h         = vr.height;
		m->radar_pending_format    = vr.dxgi_fmt;
		m->radar_pending_transform = transform;
		m->radar_pending_map       = map_name;
	}
	m->radar_has_pending.store( true );
}

void c_misc::on_render( )
{
	keybind_list( );
	dark_mode_flash( );
	render_screen_dim( );
	render_spread( );
	render_spectator_list( );
	render_bomb_timer( );
	render_radar( );
}

void c_misc::on_entity_update( const float& t )
{
	static auto off_globals = cs2_dumper::offsets::client_dll::dwGlobalVars;
	uintptr_t g_globals = memory->read<uintptr_t>( sdk->client_dll( ).base + off_globals );

	auto map_name = memory->read_string( memory->read<uintptr_t>( g_globals + 0x188 ) );
	static int old_map_name_hash = 0;

	auto hashed_map_name = FNV1A::Hash( map_name.data( ) );
	if ( hashed_map_name != old_map_name_hash ) 
	{
		if ( FNV1A::HashConst( "<empty>" ) == hashed_map_name || map_name.empty( ) )
		{
			is_in_game.store( false );
			radar_needs_clear.store( true );
			radar_last_map.clear( );
		}
		else
		{
			is_in_game.store( true );

			if ( config_variables->misc.radar_enabled->value )
			{
				std::string clean = map_name;
				auto sl = clean.rfind( '/' );
				if ( sl != std::string::npos ) clean = clean.substr( sl + 1 );
				sl = clean.rfind( '\\' );
				if ( sl != std::string::npos ) clean = clean.substr( sl + 1 );
				const auto dot = clean.rfind( '.' );
				if ( dot != std::string::npos ) clean = clean.substr( 0, dot );

				if ( clean != radar_last_map )
				{
					radar_last_map = clean;
					radar_needs_clear.store( true );
					log_dbg( "radar: loading for map '%s'", clean.c_str( ) );
					std::thread( [ this, clean ]( ) { load_radar_for_map( this, clean ); } ).detach( );
				}
			}
		}

		old_map_name_hash = hashed_map_name;
	}

	if ( config_variables->misc.radar_enabled->value && is_in_game.load( ) )
	{
		static const auto off_origin = schema_system->map_offsets[ FNV1A::HashConst( "CGameSceneNode->m_vecOrigin" ) ];

		const auto& local = entity_cache->get_local_player( );
		if ( local.pawn && local.game_scene_node )
		{
			const vector3d origin = memory->read<vector3d>(
				reinterpret_cast<uintptr_t>( local.game_scene_node ) + off_origin );
			radar_local_x.store( origin.x, std::memory_order_relaxed );
			radar_local_y.store( origin.y, std::memory_order_relaxed );
			if ( local.controller )
				radar_local_yaw.store( local.controller->cmd_angles( ).y, std::memory_order_relaxed );
			radar_local_valid.store( true, std::memory_order_relaxed );
		}
		else
		{
			radar_local_valid.store( false, std::memory_order_relaxed );
		}

		static float last_dot_update = 0.f;
		if ( t - last_dot_update >= 0.033f )
		{
			last_dot_update = t;

			std::vector<c_radar_dot> new_dots;
			new_dots.reserve( 32 );

			const int esp_front  = esp->player_front.load( );
			const unsigned count = esp->player_buf_size[ esp_front ];
			for ( unsigned i = 0; i < count; ++i )
			{
				const auto& p = esp->player_buf[ esp_front ][ i ];
				new_dots.push_back( { p.origin, p.team } );
			}

			{
				std::lock_guard lock( entity_read_mutex );
				radar_dots       = std::move( new_dots );
				radar_local_team = local.pawn ? local.pawn->team_num( ) : 0;
			}
		}
	}

	cached_tickcount = memory->read<int>( g_globals + 0x44 );
	cached_curtime = cached_tickcount * 0.015625f;

	auto& local = entity_cache->get_local_player( );
	if ( local.controller && local.pawn )
	{
		if ( config_variables->misc.dark_mode_flash->value )
		{
			static auto off_flash_time = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawnBase->m_flFlashBangTime" ) ];
			static auto off_flash_dur = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawnBase->m_flFlashDuration" ) ];
			float flash_time = 0.f, flash_dur = 0.f;
			{
				c_read_batch batch;
				batch.queue<float>( reinterpret_cast< uintptr_t >( local.pawn ) + off_flash_time, &flash_time )
					.queue<float>( reinterpret_cast< uintptr_t >( local.pawn ) + off_flash_dur, &flash_dur );
				memory->flush_batch( batch );
			}
			float time = flash_time - cached_curtime;
			flash_alpha = ( time > 0.f && flash_dur > 0.f ) ? ( time / flash_dur ) : 0.f;
		}

		if ( config_variables->misc.spectator_list->value )
		{
			static auto off_observer_services = schema_system->map_offsets[ FNV1A::HashConst( "C_BasePlayerPawn->m_pObserverServices" ) ];
			static auto off_observer_target = schema_system->map_offsets[ FNV1A::HashConst( "CPlayer_ObserverServices->m_hObserverTarget" ) ];

			static float last_update = 0.f;
			if ( t - last_update >= 0.1f )
			{
				last_update = t;

				const auto& players = entity_cache->get_players( );
				const size_t count = players.size( );

				static std::vector<c_player_observer_service*> obs_services;
				static std::vector<c_base_handle> target_handles;

				obs_services.assign( count, nullptr );
				target_handles.assign( count, c_base_handle {} );

				{
					c_read_batch batch;
					for ( size_t i = 0; i < count; ++i )
					{
						if ( !players[ i ].pawn )
							continue;

						batch.queue<c_player_observer_service*>( reinterpret_cast< uintptr_t >( players[ i ].pawn ) + off_observer_services, &obs_services[ i ] );
					}
					memory->flush_batch( batch );
				}

				{
					c_read_batch batch;
					for ( size_t i = 0; i < count; ++i )
					{
						if ( !obs_services[ i ] )
							continue;

						batch.queue<c_base_handle>( reinterpret_cast< uintptr_t >( obs_services[ i ] ) + off_observer_target, &target_handles[ i ] );
					}
					memory->flush_batch( batch );
				}

				std::vector<c_base_entity*> target_pawns;
				entity_helper->batch_get_from_handles( target_handles, target_pawns );

				std::vector<std::string> new_spectators;
				for ( size_t i = 0; i < target_pawns.size( ); ++i )
				{
					if ( !players[ i ].controller )
						continue;

					if ( target_pawns[ i ] != reinterpret_cast< c_base_entity* >( local.pawn ) )
						continue;

					std::string name = players[ i ].controller->get_name( );
					if ( !name.empty( ) )
						new_spectators.push_back( std::move( name ) );
				}

				spectators = std::move( new_spectators );
			}
		}
	}
	else if ( !spectators.empty( ) )
	{
		std::lock_guard lock( entity_read_mutex );
		spectators.clear( );
	}

	if ( config_variables->misc.bomb_timer->value )
	{
		static auto off_ticking = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_bBombTicking" ) ];
		static auto off_site = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_nBombSite" ) ];
		static auto off_blow = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_flC4Blow" ) ];
		static auto off_timer_length = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_flTimerLength" ) ];
		static auto off_defuse_length = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_flDefuseLength" ) ];
		static auto off_being_defused = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_bBeingDefused" ) ];
		static auto off_defuse_cd = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_flDefuseCountDown" ) ];
		static auto off_defused = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_bBombDefused" ) ];
		static auto off_exploded = schema_system->map_offsets[ FNV1A::HashConst( "C_PlantedC4->m_bHasExploded" ) ];

		static float last_bomb_update = 0.f;
		if ( t - last_bomb_update >= 0.1f )
		{
			last_bomb_update = t;

			static c_base_entity* cached_bomb_entity = nullptr;
			static int cached_bomb_idx = -1;

			int current_bomb_idx = esp->planted_bomb_idx.load( );
			if ( current_bomb_idx != cached_bomb_idx )
			{
				cached_bomb_idx = current_bomb_idx;
				cached_bomb_entity = ( current_bomb_idx >= 0 ) ? entity_helper->get_from_index( current_bomb_idx ) : nullptr;
			}

			if ( cached_bomb_entity )
			{
				c_bomb_info info;
				info.valid = true;

				c_read_batch batch;
				batch.queue<bool>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_ticking, &info.ticking )
					.queue<int>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_site, &info.site )
					.queue<float>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_blow, &info.blow_time )
					.queue<float>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_timer_length, &info.timer_length )
					.queue<float>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_defuse_length, &info.defuse_length )
					.queue<bool>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_being_defused, &info.being_defused )
					.queue<float>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_defuse_cd, &info.defuse_countdown )
					.queue<bool>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_defused, &info.defused )
					.queue<bool>( reinterpret_cast< uintptr_t >( cached_bomb_entity ) + off_exploded, &info.exploded );
				memory->flush_batch( batch );

				{
					std::lock_guard lock( bomb_mutex );
					bomb_info = info;
				}
			}
			else
			{
				std::lock_guard lock( bomb_mutex );
				bomb_info = {};
			}
		}
	}


}

void c_misc::render_screen_dim( )
{
	if ( !config_variables->misc.screen_dim->value )
		return;

	const int amount = config_variables->misc.screen_dim_amount->value;
	if ( amount <= 0 )
		return;

	vector2d size = window_handler->window_size;
	const int alpha = ( int ) ( amount / 135.f * 255.f );
	render->rect( 0, 0, size.x, size.y, color( 0, 0, 0, alpha ), true );
}

void c_misc::dark_mode_flash( )
{
	if ( !config_variables->misc.dark_mode_flash->value )
		return;

	if ( flash_alpha <= 0.f )
		return;

	if ( !is_in_game.load( ) )
		return;

	vector2d size = window_handler->window_size;
	render->rect( 0, 0, size.x, size.y, color( 0, 0, 0, static_cast<int>( flash_alpha * 255.f ) ), true );
}

void c_misc::keybind_list( )
{
	if ( !config_variables->misc.keybind_list->value )
		return;

	struct kb_entry
	{
		std::string name, key; int activation_type; bool active;
		int name_w = 0, key_w = 0, state_w = 0;
	};
	static std::vector<kb_entry> entries;
	entries.clear( );

	for ( auto* var : config_variables->all_variables )
	{
		if ( var->get_type( ) != config_var_type::keybind ) 
			continue;
		auto* kb = static_cast< c_keybind_variable* >( var );
		if ( kb->key == 0 || kb->activation_type == 0 ) 
			continue;

		std::string name;
		if ( kb->display_name[ 0 ] != '\0' )
			name = kb->display_name;
		else
		{
			name = kb->variable_name;
			auto dot = name.find( '.' );
			if ( dot != std::string::npos ) name = name.substr( 0, dot );
		}
		entries.push_back( { name, "[" + input_handler->get_key_name( kb->key ) + "]", kb->activation_type, kb->active } );
	}

	if ( entries.empty( ) ) return;

	const bool preview = menu->is_open;
	const bool any_active = std::any_of( entries.begin( ), entries.end( ), []( const kb_entry& e ) { return e.active; } );
	if ( !any_active && !preview ) 
		return;

	static const int header_w = ( int ) draw_pool->font_smooth.text_extent( std::string( "keybinds" ) ).x;
	static const int fh       = ( int ) draw_pool->font_smooth.text_extent( std::string( "A" ) ).y;
	int content_w = header_w;

	for ( auto& e : entries )
	{
		const char* state = e.activation_type == 1 ? "held" : "toggle";
		e.name_w  = ( int ) draw_pool->font_smooth.text_extent( e.name ).x;
		e.key_w   = ( int ) draw_pool->font_smooth.text_extent( e.key ).x;
		e.state_w = ( int ) draw_pool->font_smooth.text_extent( std::string( state ) ).x;
		content_w = max( content_w, e.name_w + 8 + e.key_w + 16 + e.state_w );
	}

	kbl_w = content_w + 20;
	kbl_h = 26 + ( int ) entries.size( ) * 16 + 6;

	const int x = ( int ) config_variables->misc.keybind_list_x->value;
	const int y = ( int ) config_variables->misc.keybind_list_y->value;
	const color& ac = config_variables->misc.widget_accent_color->value;

	render->depth_rect( x, y, kbl_w, kbl_h, color( 10, 10, 10, 240 ), true, ac, 2 );
	render->rect( x, y, kbl_w, kbl_h, color( 0, 0, 0 ), false );
	render->text( x + 8, y + 5, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, "keybinds" );
	if ( config_variables->misc.widget_solid_separator->value )
		render->rect( x + 2, y + 22, kbl_w - 4, 1, color( ac.r, ac.g, ac.b, 80 ), true );
	else
		render->gradient_line( x + 2, y + 22, x + kbl_w - 2, y + 22,
			color( ac.r, ac.g, ac.b, 0 ), ac, color( ac.r, ac.g, ac.b, 0 ), 1.f );

	for ( int i = 0; i < ( int ) entries.size( ); ++i )
	{
		const int ty      = y + 26 + i * 16;
		const auto& e     = entries[ i ];
		const char* state = e.activation_type == 1 ? "held" : "toggle";

		if ( e.active )
			render->rect( x + 4, ty, 2, fh, ac, true );

		render->text( x + 10, ty, draw_pool->font_smooth,
			e.active ? color( 255, 255, 255 ) : color( 65, 65, 65 ),
			daisy::TEXT_ALIGN_DEFAULT, false, e.name.c_str( ) );
		render->text( x + 10 + e.name_w + 8, ty, draw_pool->font_smooth,
			e.active ? color( ac.r, ac.g, ac.b, 200 ) : color( 40, 40, 40 ),
			daisy::TEXT_ALIGN_DEFAULT, false, e.key.c_str( ) );
		render->text( x + kbl_w - 10 - e.state_w, ty, draw_pool->font_smooth,
			color( 55, 55, 55 ), daisy::TEXT_ALIGN_DEFAULT, false, state );
	}
}

void c_misc::render_spread( )
{
	if ( !config_variables->misc.spread_indicator->value )
		return;

	if ( !is_in_game.load( ) )
		return;

	auto& local = entity_cache->get_local_player( );
	if ( !local.pawn || !local.controller )
		return;

	auto weapon = local.pawn->active_weapon( );
	if ( !weapon )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata )
		return;

	static int last_tickcount = -1;

	struct tick_result_t
	{
		vector2d offset = {};
		bool on_target = false;
	};

	static std::array<tick_result_t, 2> tick_results {};
	static float cached_inaccuracy = 0.f;
	static float cached_spread = 0.f;

	int tickcount = cached_tickcount;
	if ( tickcount != last_tickcount )
	{
		last_tickcount = tickcount;

		vector3d cmd_angles = local.controller->cmd_angles( );
		cached_inaccuracy = weapon->get_inaccuracy( local.pawn, vdata );
		cached_spread = vdata->weapon_spread( );
		const float recoil_index = weapon->recoil_index( );
		const int item_def_idx = weapon->item_definition_index( );
		const int num_bullets = vdata->num_bullets( );
		const float range = vdata->weapon_range( );

		const float deg2rad = std::numbers::pi_v<float> / 180.f;
		const float sp = std::sinf( cmd_angles.x * deg2rad );
		const float cp = std::cosf( cmd_angles.x * deg2rad );
		const float sy = std::sinf( cmd_angles.y * deg2rad );
		const float cy = std::cosf( cmd_angles.y * deg2rad );
		const vector3d fwd { cp * cy, cp * sy, -sp };
		const vector3d rgt { -sy, cy, 0.f };
		const vector3d up { sp * cy, sp * sy, cp };

		vector3d eye_pos = local.pawn->get_eye_position( );
		auto players = entity_cache->get_players( );
		int local_team = local.pawn->team_num( );

		for ( int tick_offset = 0; tick_offset < 2; ++tick_offset )
		{
			const uint32_t tick_seed = math::get_server_seed( cmd_angles, tickcount + tick_offset );
			vector2d sv = math::calc_spread( tick_seed + 1, cached_inaccuracy, cached_spread, recoil_index, item_def_idx, num_bullets );

			tick_results[ tick_offset ].offset = sv;
			tick_results[ tick_offset ].on_target = false;

			vector3d direction = fwd + ( rgt * -sv.x ) + ( up * sv.y );
			direction.normalize_in_place( );

			for ( const auto& player : players )
			{
				if ( !player.is_valid( ) || !player.pawn->is_alive( ) )
					continue;

				if ( player.pawn->team_num( ) == local_team && !global_vars->team_cvar( ) )
					continue;

				vector3d mins = player.pawn->as_ent( )->collision( )->mins( );
				vector3d maxs = player.pawn->as_ent( )->collision( )->maxs( );
				mins += player.game_scene_node->origin( );
				maxs += player.game_scene_node->origin( );

				ray_t ray {};
				ray.init( eye_pos, eye_pos + direction * range, player.pawn );
				if ( trace->does_ray_hit_player( ray, nullptr, mins, maxs, false ) )
				{
					tick_results[ tick_offset ].on_target = true;
					break;
				}
			}
		}
	}

	using clock = std::chrono::steady_clock;
	using fsec  = std::chrono::duration<float>;
	static float prev_t = 0.f;
	float cur_t = std::chrono::duration_cast<fsec>( clock::now( ).time_since_epoch( ) ).count( );
	float dt = min( cur_t - prev_t, 0.1f );
	prev_t = cur_t;

	vector2d screen = window_handler->window_size;
	const float cx = screen.x * 0.5f;
	const float cy = screen.y * 0.5f;
	const float fov_y = 2.f * std::atanf( std::tanf( deg_2_rad( 90.f ) * 0.5f ) * ( screen.y / screen.x ) );
	const float scale = ( screen.y * 0.5f ) / std::tanf( fov_y * 0.5f );
	const float radius = ( cached_inaccuracy + cached_spread ) * scale;

	static float display_radius = 0.f;
	display_radius += ( radius - display_radius ) * min( dt * 14.f, 1.f );

	const color base_clr = config_variables->misc.spread_indicator_color->value;
	const color col_success = config_variables->misc.spread_indicator_dot_color_success->value;
	const color col_default = config_variables->misc.spread_indicator_dot_color->value;

	const float inaccuracy_t = std::clamp( cached_inaccuracy / 0.05f, 0.f, 1.f );
	const color circle_clr(
		base_clr.r,
		( byte ) std::lerp( ( float ) base_clr.g, 80.f,  inaccuracy_t ),
		( byte ) std::lerp( ( float ) base_clr.b, 20.f,  inaccuracy_t ),
		base_clr.a
	);

	render->circle_aa( cx, cy, display_radius, 64, circle_clr );

	render->rect( ( int ) cx - 1, ( int ) cy - 1, 3, 3, color( 0, 0, 0, 180 ), true );
	render->rect( ( int ) cx,     ( int ) cy,     1, 1, color( base_clr.r, base_clr.g, base_clr.b, base_clr.a ), true );

	for ( int i = 0; i < 2; ++i )
	{
		const auto& r = tick_results[ i ];
		const float sx = cx + r.offset.x * -scale;
		const float sy = cy + r.offset.y * scale;
		const color dot = r.on_target ? col_success : col_default;
		const int dot_r = i == 0 ? 2 : 1;
		render->rect( ( int ) sx - dot_r - 1, ( int ) sy - dot_r - 1, dot_r * 2 + 2, dot_r * 2 + 2, color( 0, 0, 0, 150 ), true );
		render->rect( ( int ) sx - dot_r,     ( int ) sy - dot_r,     dot_r * 2,     dot_r * 2,     dot, true );
	}
}

void c_misc::render_spectator_list( )
{
	if ( !config_variables->misc.spectator_list->value )
		return;

	std::vector<std::string> local_spectators;
	{
		std::lock_guard lock( entity_read_mutex );
		local_spectators = spectators;
	}

	const bool preview = menu->is_open;
	if ( local_spectators.empty( ) && !preview ) return;

	static const int fh          = ( int ) draw_pool->font_smooth.text_extent( std::string( "A" ) ).y;
	static const int ellipsis_w  = ( int ) draw_pool->font_smooth.text_extent( std::wstring( L"..." ) ).x;
	constexpr int    max_name_w  = 220;

	char header_buf[ 32 ];
	if ( local_spectators.empty( ) )
		sprintf_s( header_buf, "spectators" );
	else
		sprintf_s( header_buf, "spectators (%d)", ( int ) local_spectators.size( ) );

	int content_w = ( int ) draw_pool->font_smooth.text_extent( std::string( header_buf ) ).x;

	std::vector<std::wstring> wide_names( local_spectators.size( ) );
	for ( size_t i = 0; i < local_spectators.size( ); ++i )
	{
		wide_names[ i ] = global_vars->utf8_to_wide( local_spectators[ i ] );
		int w = ( int ) draw_pool->font_smooth.text_extent( wide_names[ i ] ).x;
		if ( w > max_name_w )
		{
			std::wstring truncated;
			for ( wchar_t c : wide_names[ i ] )
			{
				if ( ( int ) draw_pool->font_smooth.text_extent( truncated + c ).x + ellipsis_w > max_name_w )
					break;
				truncated += c;
			}
			wide_names[ i ] = truncated + L"...";
			w = min( ( int ) draw_pool->font_smooth.text_extent( wide_names[ i ] ).x, max_name_w );
		}
		content_w = max( content_w, w );
	}

	const int list_w = content_w + 8 + 4;
	const int list_h = local_spectators.empty( ) ? 32 : 26 + ( int ) local_spectators.size( ) * 16 + 6;
	const int x = ( int ) config_variables->misc.spectator_list_x->value;
	const int y = ( int ) config_variables->misc.spectator_list_y->value;
	const color& ac = config_variables->misc.widget_accent_color->value;

	render->depth_rect( x, y, list_w, list_h, color( 10, 10, 10, 240 ), true, ac, 2 );
	render->rect( x, y, list_w, list_h, color( 0, 0, 0 ), false );
	render->text( x + 8, y + 5, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, header_buf );
	if ( config_variables->misc.widget_solid_separator->value )
		render->rect( x + 2, y + 22, list_w - 4, 1, color( ac.r, ac.g, ac.b, 80 ), true );
	else
		render->gradient_line( x + 2, y + 22, x + list_w - 2, y + 22,
			color( ac.r, ac.g, ac.b, 0 ), ac, color( ac.r, ac.g, ac.b, 0 ), 1.f );

	for ( int i = 0; i < ( int ) local_spectators.size( ); ++i )
	{
		const int ty = y + 26 + i * 16;
		render->rect( x + 4, ty, 2, fh, color( ac.r, ac.g, ac.b, 140 ), true );
		render->text_w( x + 8, ty, draw_pool->font_smooth,
			color( 220, 220, 220 ), daisy::TEXT_ALIGN_DEFAULT, false, wide_names[ i ].c_str( ) );
	}
}

void c_misc::render_bomb_timer( )
{
	if ( !config_variables->misc.bomb_timer->value )
		return;

	c_bomb_info info;
	{
		std::lock_guard lock( bomb_mutex );
		info = bomb_info;
	}

	const bool preview = menu->is_open;
	if ( !info.valid && !preview ) return;
	if ( !preview && !is_in_game.load( ) ) return;

	const float curtime    = cached_curtime;
	const int   bx         = ( int ) config_variables->misc.bomb_timer_x->value;
	const int   by         = ( int ) config_variables->misc.bomb_timer_y->value;
	const int   bar_w      = bomb_w - 20;
	const int   bar_h      = 5;
	const bool  show_defuse = info.valid && info.being_defused;

	static const int fh = ( int ) draw_pool->font_smooth.text_extent( std::string( "A" ) ).y;

	if ( !info.valid )
		bomb_h = 32;
	else
	{
		bomb_h = 26 + fh + 6 + bar_h + 6;
		if ( show_defuse ) bomb_h += 4 + fh + 4 + bar_h + 4;
	}

	const color& ac = config_variables->misc.widget_accent_color->value;

	render->depth_rect( bx, by, bomb_w, bomb_h, color( 10, 10, 10, 240 ), true, ac, 2 );
	render->rect( bx, by, bomb_w, bomb_h, color( 0, 0, 0 ), false );

	const char* site_str = info.valid ? ( info.site == 0 ? "bomb [A]" : "bomb [B]" ) : "bomb timer";
	render->text( bx + 8, by + 5, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, site_str );
	if ( config_variables->misc.widget_solid_separator->value )
		render->rect( bx + 2, by + 22, bomb_w - 4, 1, color( ac.r, ac.g, ac.b, 80 ), true );
	else
		render->gradient_line( bx + 2, by + 22, bx + bomb_w - 2, by + 22,
			color( ac.r, ac.g, ac.b, 0 ), ac, color( ac.r, ac.g, ac.b, 0 ), 1.f );

	if ( !info.valid ) 
		return;

	const int cx = bx + 10;
	int cy = by + 26;

	float time_left = 0.f;
	if ( !info.defused && !info.exploded )
	{
		if ( info.ticking )
		{
			time_left = info.blow_time - curtime;
			if ( time_left < 0.f ) { info.exploded = true; time_left = 0.f; }
		}
		else
			time_left = info.timer_length;
	}

	const color bar_color  = config_variables->misc.bomb_timer_color->value;
	const bool  urgent     = time_left < 10.f && !info.defused && !info.exploded;
	const color time_clr   = urgent ? color( 255, 80, 60 ) : color( 255, 255, 255 );

	char time_buf[ 16 ];
	if ( info.defused )       sprintf_s( time_buf, "defused" );
	else if ( info.exploded ) sprintf_s( time_buf, "exploded" );
	else                      sprintf_s( time_buf, "%.1fs", time_left );

	render->text( cx, cy, draw_pool->font_smooth, time_clr, daisy::TEXT_ALIGN_DEFAULT, false, time_buf );
	cy += fh + 4;

	const float time_frac = info.timer_length > 0.f ? max( 0.f, min( 1.f, time_left / info.timer_length ) ) : 0.f;
	render->rect( cx, cy, bar_w, bar_h, color( 20, 20, 20 ), true );
	if ( time_frac > 0.f )
	{
		const int fill_w = ( int ) ( bar_w * time_frac );
		render->gradient_rect( cx, cy, fill_w, bar_h,
			color( bar_color.r / 2, bar_color.g / 2, bar_color.b / 2, bar_color.a ), bar_color, false );
	}
	cy += bar_h + 4;

	if ( show_defuse )
	{
		float defuse_left = max( 0.f, info.defuse_countdown - curtime );
		const float defuse_frac = info.defuse_length > 0.f ? max( 0.f, min( 1.f, defuse_left / info.defuse_length ) ) : 0.f;

		char def_buf[ 16 ];
		sprintf_s( def_buf, "defuse %.1fs", defuse_left );
		render->text( cx, cy, draw_pool->font_smooth, color( 100, 200, 255 ), daisy::TEXT_ALIGN_DEFAULT, false, def_buf );
		cy += fh + 4;
		render->rect( cx, cy, bar_w, bar_h, color( 20, 20, 20 ), true );
		if ( defuse_frac > 0.f )
		{
			const int fill_w = ( int ) ( bar_w * ( 1.f - defuse_frac ) );
			render->gradient_rect( cx, cy, fill_w, bar_h, color( 50, 100, 130 ), color( 100, 200, 255 ), false );
		}
	}
}

void c_misc::render_radar( )
{
	if ( !config_variables->misc.radar_enabled->value )
		return;

	if ( !is_in_game.load( ) && !menu->is_open )
		return;

	if ( radar_needs_clear.exchange( false ) )
	{
		if ( radar_srv ) { radar_srv->Release( ); radar_srv = nullptr; }
		radar_loaded_map.clear( );
		radar_transform = {};
	}

	if ( radar_has_pending.load( ) )
	{
		std::lock_guard lock( radar_mutex );
		if ( !radar_pending_rgba.empty( ) && radar_pending_w > 0 )
		{
			if ( radar_srv ) { radar_srv->Release( ); radar_srv = nullptr; }

			if ( radar_pending_format == DXGI_FORMAT_R8G8B8A8_UNORM )
				radar_srv = draw_pool->create_srv_from_rgba(
					radar_pending_rgba.data( ), radar_pending_w, radar_pending_h );
			else
				radar_srv = draw_pool->create_srv_from_bcn(
					radar_pending_rgba.data( ), radar_pending_w, radar_pending_h,
					radar_pending_format );

			if ( radar_srv )
			{
				radar_tex_w      = radar_pending_w;
				radar_tex_h      = radar_pending_h;
				radar_transform  = radar_pending_transform;
				radar_loaded_map = radar_pending_map;
			}
			radar_pending_rgba.clear( );
		}
		radar_has_pending.store( false );
	}

	if ( !radar_srv && is_in_game.load( ) )
		return;

	constexpr int header_h = 28;
	constexpr int pad      = 2;

	const int rx      = static_cast<int>( config_variables->misc.radar_x->value );
	const int ry      = static_cast<int>( config_variables->misc.radar_y->value );
	const int rs      = config_variables->misc.radar_size->value;
	const int total_w = rs + pad * 2;
	const int total_h = header_h + rs + pad;
	const int img_x   = rx + pad;
	const int img_y   = ry + header_h;

	const color& ac = config_variables->misc.widget_accent_color->value;

	render->depth_rect( rx, ry, total_w, total_h, color( 10, 10, 10, 240 ), true, ac, 2 );
	render->rect( rx, ry, total_w, total_h, color( 0, 0, 0 ), false );
	render->text( rx + 8, ry + 5, draw_pool->font_smooth, color( 255, 255, 255 ), daisy::TEXT_ALIGN_DEFAULT, true, "radar" );

	if ( config_variables->misc.widget_solid_separator->value )
		render->rect( rx + 2, ry + 22, total_w - 4, 1, color( ac.r, ac.g, ac.b, 80 ), true );
	else
		render->gradient_line( rx + 2, ry + 22, rx + total_w - 2, ry + 22,
			color( ac.r, ac.g, ac.b, 0 ), ac, color( ac.r, ac.g, ac.b, 0 ), 1.f );

	static std::vector<c_radar_dot> dots;
	int local_team = 0;
	{
		std::lock_guard lock( entity_read_mutex );
		dots       = radar_dots;
		local_team = radar_local_team;
	}

	const float full_range  = radar_transform.scale * 1024.f;
	const float zoom        = max( 1.f, config_variables->misc.radar_zoom->value );
	const bool  ffa         = global_vars->team_cvar( );
	const bool  local_valid = radar_local_valid.load( std::memory_order_relaxed );
	const float lx          = local_valid ? radar_local_x.load( std::memory_order_relaxed ) : 0.f;
	const float ly          = local_valid ? radar_local_y.load( std::memory_order_relaxed ) : 0.f;
	const bool  rotate      = config_variables->misc.radar_rotate->value;

	const float yaw_rad = ( rotate && local_valid )
	    ? deg_2_rad( radar_local_yaw.load( std::memory_order_relaxed ) )
	    : ( std::numbers::pi_v<float> * 0.5f );
	const float sin_yaw     = std::sin( yaw_rad );
	const float cos_yaw     = std::cos( yaw_rad );

	float cu = 0.5f, cv = 0.5f;
	if ( full_range > 0.f && local_valid )
	{
		cu = ( lx - radar_transform.x ) / full_range;
		cv = ( radar_transform.y - ly ) / full_range;
	}

	const float half = 0.5f / zoom;
	cu = std::clamp( cu, half, 1.f - half );
	cv = std::clamp( cv, half, 1.f - half );

	render->set_viewport( img_x, img_y, rs, rs );

	if ( radar_srv )
	{
		const float h  = rs * 0.5f;
		const float sc = 1.f / ( rs * zoom );

		auto corner_uv = [&]( float px, float py ) -> daisy::point_t {
			return { cu + ( px * sin_yaw - py * cos_yaw ) * sc,
			         cv + ( px * cos_yaw + py * sin_yaw ) * sc };
		};

		const daisy::point_t pos_tl { ( float ) img_x,      ( float ) img_y      };
		const daisy::point_t pos_tr { ( float ) img_x + rs,  ( float ) img_y      };
		const daisy::point_t pos_br { ( float ) img_x + rs,  ( float ) img_y + rs };
		const daisy::point_t pos_bl { ( float ) img_x,       ( float ) img_y + rs };

		const daisy::point_t uv_tl = corner_uv( -h, -h );
		const daisy::point_t uv_tr = corner_uv( +h, -h );
		const daisy::point_t uv_br = corner_uv( +h, +h );
		const daisy::point_t uv_bl = corner_uv( -h, +h );

		const daisy::color_t white( 255, 255, 255, 255 );

		draw_pool->buffer->queue.push_pixel_shader( daisy::daisy_t::s_image_pixel_shader );
		draw_pool->buffer->queue.push_sampler( daisy::daisy_t::s_linear_sampler_state );
		draw_pool->buffer->queue.push_filled_triangle( pos_tl, pos_tr, pos_bl, white, white, white, radar_srv, uv_tl, uv_tr, uv_bl );
		draw_pool->buffer->queue.push_filled_triangle( pos_tr, pos_br, pos_bl, white, white, white, radar_srv, uv_tr, uv_br, uv_bl );
		draw_pool->buffer->queue.push_sampler( daisy::daisy_t::s_sampler_state );
		draw_pool->buffer->queue.push_pixel_shader( daisy::daisy_t::s_pixel_shader );
	}
	else
	{
		render->rect( img_x, img_y, rs, rs, color( 20, 20, 20, 200 ), true );
		render->text( img_x + rs / 2, img_y + rs / 2,
		              draw_pool->font_smooth, color( 80, 80, 80, 200 ),
		              (daisy::daisy_text_align)( daisy::TEXT_ALIGNX_CENTER | daisy::TEXT_ALIGNY_CENTER ), false, "paste failed" );
	}

	if ( full_range > 0.f && radar_srv )
	{
		const float pix_per_world = ( rs * zoom ) / full_range;
		const float cx    = radar_transform.x + cu * full_range;
		const float cy    = radar_transform.y - cv * full_range;
		const float dot_r = rs * 0.0108f;
		const float cx_px = img_x + rs * 0.5f;
		const float cy_px = img_y + rs * 0.5f;
		const float edge  = rs * 0.5f - dot_r - 2.f;

		for ( const auto& dot : dots )
		{
			const color clr = ( !ffa && local_team != 0 && dot.team == local_team )
				? color( 60, 220, 100, 255 )
				: color( 220, 60, 60, 255 );

			const float raw_x = ( dot.origin.x - cx ) * pix_per_world;
			const float raw_y = ( cy - dot.origin.y ) * pix_per_world;
			const float rel_x = raw_x * sin_yaw + raw_y * cos_yaw;
			const float rel_y = -raw_x * cos_yaw + raw_y * sin_yaw;

			if ( std::fabs( rel_x ) <= edge && std::fabs( rel_y ) <= edge )
			{
				render->circle_filled( ( int )( cx_px + rel_x ), ( int )( cy_px + rel_y ), dot_r, 12, clr );
			}
			else
			{
				const float abs_x = std::fabs( rel_x ), abs_y = std::fabs( rel_y );
				const float t     = edge / ( abs_x >= abs_y ? abs_x : abs_y );
				render->circle_filled( ( int )( cx_px + rel_x * t ), ( int )( cy_px + rel_y * t ), dot_r, 12, clr );
			}
		}

		if ( local_valid )
		{
			const float raw_lx = ( lx - cx ) * pix_per_world;
			const float raw_ly = ( cy - ly ) * pix_per_world;
			const float lrel_x = raw_lx * sin_yaw + raw_ly * cos_yaw;
			const float lrel_y = -raw_lx * cos_yaw + raw_ly * sin_yaw;
			render->circle_filled( ( int )( cx_px + lrel_x ), ( int )( cy_px + lrel_y ), dot_r, 12, color( 255, 255, 255, 255 ) );
		}
	}

	render->restore_viewport( );
}