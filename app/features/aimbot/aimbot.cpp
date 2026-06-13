#include "aimbot.h"
#include "..\..\renderer\renderer.h"
#include "..\..\renderer\window_handler.h"
#include "..\menu\input\input.h"
#include <algorithm>    // std::max

#include <random>
#include "..\mouse\mouse.h"
#include "..\rcs\rcs.h"
#include "..\autowall\autowall.h"
#include "..\menu\menu.h"
#include "..\smoke\smoke.h"
#include <numbers>

static inline uint32_t seed = 486321;
static inline uint32_t lcg( )
{
	seed = 1664525 * seed + 1013904223;
	return seed;
}

static inline int random( int min, int max )
{
	return min + ( lcg( ) % ( max - min + 1 ) );
}

void c_aimbot::on_render( )
{
	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.is_valid( ) )
		return;

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( local_player.weapon_service->active_weapon_handle( ) );
	if ( !weapon )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata || vdata->melee_weapon( ) )
		return;

	if ( !config_variables->aimbot.fov_circle->value )
		return;

	vector2d size = window_handler->window_size;
	if ( size.x <= 0.f || size.y <= 0.f )
		return;

	if ( base_fov <= 0.f )
		return;

	float radius = base_fov * size.y / 180.f;
	render->circle_aa( size.x * 0.5f, size.y * 0.5f, radius, 150, config_variables->aimbot.fov_circle_color->value );
}

// epoic fuynction no 100000000123221435435345% cpu fixerd by VOIDZERO_HVH_LEGENDE <------- facts
void normalize_da_angle(float& angle)
{
	float rot;

	// bad number.
	if (!std::isfinite(angle))
	{
		angle = 0.f;
		return;
	}

	// no need to normalize this angle.
	if (angle >= -180.f && angle <= 180.f)
		return;

	// get amount of rotations needed.
	rot = std::round(std::abs(angle / 360.f));

	// normalize.
	angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
}

void c_aimbot::setup_weapon_config( )
{
	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.is_valid( ) )
		return;

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( local_player.weapon_service->active_weapon_handle( ) );
	if ( !weapon )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata )
		return;

	const auto apply_weapon_config = [ & ] ( const c_config_variables::c_aimbot::c_weapon_config_base& cfg, bool is_global = false )
		{
			if ( !is_global && !cfg.override_global->value )
				return;

			base_fov = cfg.fov->value;
			settings.max_fov = base_fov;
			settings.autowall_enabled = cfg.autowall->value;
			settings.autowall_damage = cfg.mindamage->value;
			settings.head = cfg.head->value;
			settings.chest = cfg.chest->value;
			settings.pelvis = cfg.pelvis->value;
			settings.legs = cfg.legs->value;
			settings.toes = cfg.toes->value;
			settings.smooth_enabled = cfg.smooth->value;
			settings.smooth_x = cfg.smooth_x->value;
			settings.smooth_y = cfg.smooth_y->value;

			settings.random_smooth_enabled = cfg.randomized_smooth->value;
			settings.randomize_smooth_x = cfg.randomized_x->value;
			settings.randomize_smooth_y = cfg.randomized_y->value;

			settings.prediction = cfg.prediction->value;
			settings.smoothing_type = cfg.smooth_type->value;
		};

	apply_weapon_config( config_variables->aimbot.global_weapon, true );

	switch ( vdata->weapon_type( ) )
	{
	case 1:
		if ( vdata->revolver( ) || weapon->item_definition_index( ) == 1 )
		{
			if ( config_variables->aimbot.heavy_pistols.override_global->value )
			{
				apply_weapon_config( config_variables->aimbot.heavy_pistols ); 
				break;
			}
		}
		else
		{
			if ( config_variables->aimbot.pistols.override_global->value )
			{
				apply_weapon_config( config_variables->aimbot.pistols ); 
				break;
			}
		}
	case 2:
		if ( config_variables->aimbot.smgs.override_global->value )
		{
			apply_weapon_config( config_variables->aimbot.smgs ); 
			break;
		}
	case 3:
		if ( config_variables->aimbot.rifles.override_global->value )
		{
			apply_weapon_config( config_variables->aimbot.rifles ); 
			break;
		}
	case 6:
	case 4:
		if ( config_variables->aimbot.heavy_rifles.override_global->value )
		{
			apply_weapon_config( config_variables->aimbot.heavy_rifles ); 
			break;
		}
	case 5:
		if ( config_variables->aimbot.snipers.override_global->value )
		{
			apply_weapon_config( config_variables->aimbot.snipers ); 
			break;
		}
	}
}

void c_aimbot::on_input( )
{
	setup_weapon_config( );

	if ( !config_variables->aimbot.enabled->value )
	{
		is_active = config_variables->aimbot.aim_key->active = false;
		locked_target = {};
		return;
	}

	switch ( config_variables->aimbot.aim_key->activation_type )
	{
	case 0:
		is_active = config_variables->aimbot.aim_key->active = true;
		break;
	case 1:
	{
		auto s = input_handler->keybind_state( config_variables->aimbot.aim_key->key );
		is_active = config_variables->aimbot.aim_key->active = ( s == PRESSED || s == HELD );
		break;
	}
	case 2:
		if ( input_handler->keybind_state( config_variables->aimbot.aim_key->key ) == PRESSED )
			config_variables->aimbot.aim_key->active = !config_variables->aimbot.aim_key->active;

		is_active = config_variables->aimbot.aim_key->active;
		break;
	}

	if ( !is_active )
		locked_target = {};

}

bool c_aimbot::verify_visibility( const vector3d& from, const vector3d& pos, const int hitbox_id ) const
{
	if ( settings.autowall_enabled )
	{
		float damage = autowall->handle_bullet_penetration( pos, from,
			autowall_input.scaled_damage, autowall_input.range_modifier, autowall_input.penetration_power, hitbox_id );
		if ( damage > settings.autowall_damage )
			return true;
	}
	return parser->is_visible( from, pos );
}

vector3d c_aimbot::predict_aimpoint( const vector3d& from, const vector3d& point, const vector3d& velocity )
{
	if ( !settings.prediction )
		return point;

	float flight_time = entity_cache->tickrate;
	return point + velocity * flight_time;
}

void c_aimbot::find_best_target( const vector2d& screen_center, const vector3d& from, vector3d& output_pos, const vector3d& view_angles )
{
	auto& local_player = entity_cache->get_local_player( );
	int local_team = local_player.pawn->team_num( );
	bool team_var = global_vars->team_cvar( );
	const std::vector<c_cached_entity>& players = entity_cache->get_players( );

	autowall_input.min_damage = settings.autowall_damage;

	struct hitbox_entry
	{
		int index {}; bool enabled {};
	};
	const hitbox_entry hitboxes[ ] =
	{
		{ HITBOX_HEAD,    settings.head },
		{ HITBOX_CHEST,   settings.chest },
		{ HITBOX_STOMACH, settings.pelvis },
		{ HITBOX_PELVIS, settings.pelvis },
		{ HITBOX_LEFT_CALF, settings.legs },
		{ HITBOX_RIGHT_CALF, settings.legs },
		{ HITBOX_LEFT_FOOT, settings.toes },
		{ HITBOX_RIGHT_FOOT, settings.toes },
	};

	static auto off_velocity = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_vecVelocity" ) ];
	static auto off_life_state = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_lifeState" ) ];
	static auto off_health = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_iHealth" ) ];
	static auto off_model_state = schema_system->map_offsets[ FNV1A::HashConst( "CSkeletonInstance->m_modelState" ) ];
	static auto off_skel_inst = schema_system->map_offsets[ FNV1A::HashConst( "CBodyComponentSkeletonInstance->m_skeletonInstance" ) ];

	const size_t n = players.size( );
	if ( !n ) return;

	struct per_player_t
	{
		int life_state = 0;
		int health = 0;
		vector3d velocity = {};
		uintptr_t bone_array = 0;
		std::array<vector3d, 28> bones = {};
	};
	static std::vector<per_player_t> pdata;
	pdata.assign( n, {} );

	{
		c_read_batch batch;
		for ( size_t i = 0; i < n; ++i )
		{
			if ( !players[ i ].pawn ) continue;
			auto base = reinterpret_cast< uintptr_t >( players[ i ].pawn );
			batch.queue<int>( base + off_life_state, &pdata[ i ].life_state )
				.queue<int>( base + off_health, &pdata[ i ].health )
				.queue<vector3d>( base + off_velocity, &pdata[ i ].velocity );
		}
		memory->flush_batch( batch );
	}

	{
		c_read_batch batch;
		for ( size_t i = 0; i < n; ++i )
		{
			if ( !players[ i ].game_scene_node ) continue;
			if ( pdata[ i ].life_state != 256 || pdata[ i ].health <= 0 ) continue;
			if ( !team_var && players[ i ].team_num == local_team ) continue;
			auto gsn = reinterpret_cast< uintptr_t >( players[ i ].game_scene_node );
			batch.queue<uintptr_t>( gsn + off_model_state + off_skel_inst, &pdata[ i ].bone_array );
		}
		memory->flush_batch( batch );
	}

	bool needed[ 28 ] = {};
	for ( auto& b : hitboxes )
		if ( b.enabled )
		{
			int bn = math::hitbox_to_bone_index( b.index );
			if ( bn >= 0 && bn < 28 ) needed[ bn ] = true;
		}

	{
		c_read_batch batch;
		for ( size_t i = 0; i < n; ++i )
		{
			if ( !pdata[ i ].bone_array )
				continue;

			for ( int b = 0; b < 28; ++b )
				if ( needed[ b ] )
					batch.queue<vector3d>( pdata[ i ].bone_array + b * sizeof( c_bone_data ), &pdata[ i ].bones[ b ] );
		}
		memory->flush_batch( batch );
	}

	static vector3d last_locked_pos = {};
	static int      locked_miss     = 0;

	if ( locked_target.pawn )
	{
		size_t li = n;
		for ( size_t i = 0; i < n; ++i )
			if ( players[ i ].pawn == locked_target.pawn ) {
				li = i; break;
			}

		if ( li < n && pdata[ li ].life_state == 256 && pdata[ li ].health > 0 && pdata[ li ].bone_array )
		{
			for ( auto& b : hitboxes )
			{
				if ( !b.enabled )
					continue;

				int bi = math::hitbox_to_bone_index( b.index );
				if ( bi < 0 )
					continue;

				vector3d hitbox_pos = predict_aimpoint( from, pdata[ li ].bones[ bi ], pdata[ li ].velocity );
				float fov_to = view_angles.fov_to( from.angle_to( hitbox_pos ) );
				if ( fov_to >= settings.max_fov )
					continue;

				if ( settings.smoke_check && smoke_system->line_through_smoke( from, hitbox_pos ) )
					continue;

				autowall_input.hitbox_id = bi;
				autowall_input.scale_damage( locked_target.pawn, b.index );
				if ( verify_visibility( from, hitbox_pos, b.index ) )
				{
					locked_miss = 0;
					last_locked_pos = output_pos = hitbox_pos;
					return;
				}
			}
		}

		if ( !last_locked_pos.is_zero( ) && ++locked_miss < 3 )
		{
			output_pos = last_locked_pos;
			return;
		}

		locked_miss = 0;
		last_locked_pos = {};
		locked_target = {};
	}

	float best_fov = settings.max_fov;
	vector3d output_best = {};

	for ( size_t i = 0; i < n; ++i )
	{
		if ( pdata[ i ].life_state != 256 || pdata[ i ].health <= 0 ) 
			continue;

		if ( !team_var && players[ i ].team_num == local_team ) 
			continue;

		if ( !pdata[ i ].bone_array ) 
			continue;

		for ( auto& b : hitboxes )
		{
			if ( !b.enabled )
				continue;

			int bi = math::hitbox_to_bone_index( b.index );
			if ( bi < 0 ) 
				continue;

			vector3d hitbox_pos = predict_aimpoint( from, pdata[ i ].bones[ bi ], pdata[ i ].velocity );
			float fov_to = view_angles.fov_to( from.angle_to( hitbox_pos ) );
			if ( fov_to >= best_fov )
				continue;

			if ( settings.smoke_check && smoke_system->line_through_smoke( from, hitbox_pos ) )
				continue;

			autowall_input.hitbox_id = bi;
			autowall_input.scale_damage( players[ i ].pawn, b.index );
			if ( verify_visibility( from, hitbox_pos, b.index ) )
			{
				output_best = hitbox_pos;
				best_fov = fov_to;
				locked_target = players[ i ];
				break;
			}
		}
	}

	if ( !output_best.is_zero( ) )
		output_pos = output_best;
}

static std::mt19937 s_rng( std::random_device {}( ) );

static vector2d generate_valid_mouse_delta(float dx, float dy)
{
	float magnitude = std::hypot(dx, dy);
	float noise_std = (magnitude * 0.05f > 0.1f) ? (magnitude * 0.05f) : 0.1f;

	std::normal_distribution<float> dist(0.f, noise_std);
	return { dx + dist(s_rng), dy + dist(s_rng) };
}

void c_aimbot::on_entity_update( const float& t )
{
	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.is_valid( ) )
		return;

	if ( menu->is_open )
		return;

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( local_player.weapon_service->active_weapon_handle( ) );
	if ( !weapon )
		return;

	if ( !entity_cache->is_fresh_game_tick( ) )
		return;

	if ( weapon->clip_1( ) <= 0 )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata || vdata->melee_weapon( ) )
		return;

	int type = vdata->weapon_type( );
	if ( type == 9 || type == 11 )
		return;

	if ( local_player.pawn->buy_menu( ) )
		return;

	int zoom_level = weapon->zoom_level( );
	if ( zoom_level > 0 )
	{
		float zoom_fov = zoom_level == 1 ? vdata->zoom_fov_1( ) : vdata->zoom_fov_2( );
		constexpr float default_fov = 90.f;
		settings.max_fov = base_fov * ( zoom_fov / default_fov );
	}
	else
	{
		settings.max_fov = base_fov;
	}

	static auto off_view_angles = cs2_dumper::offsets::client_dll::dwViewAngles;
	static auto off_view_offset = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseModelEntity->m_vecViewOffset" ) ];
	static auto off_velocity = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_vecVelocity" ) ];

	static auto off_origin = schema_system->map_offsets[ FNV1A::HashConst( "CGameSceneNode->m_vecOrigin" ) ];

	struct
	{
		vector3d view_angles = {};
		vector3d view_offset = {};
		vector3d velocity = {};
		vector3d origin = {};
	} d;

	{
		c_read_batch batch;
		batch.queue<vector3d>( sdk->client_dll( ).base + off_view_angles, &d.view_angles )
			.queue<vector3d>( reinterpret_cast< uintptr_t >( local_player.entity ) + off_view_offset, &d.view_offset )
			.queue<vector3d>( reinterpret_cast< uintptr_t >( local_player.pawn ) + off_velocity, &d.velocity )
			.queue<vector3d>( reinterpret_cast< uintptr_t >( local_player.game_scene_node ) + off_origin, &d.origin );
		memory->flush_batch( batch );
	}

	vector3d local_origin = d.origin;
	vector3d shoot_pos = local_origin + d.view_offset;
	vector3d punch = rcs->get_punch( false );

	vector2d center = {};
	window_handler->actual_size( center );
	center /= 2.f;

	vector3d aimpos = {};
	autowall_input.init( weapon );
	settings.smoke_check = config_variables->aimbot.smoke_check->value;

	find_best_target( center, shoot_pos, aimpos, d.view_angles );

	static vector2d vel = {};

	if ( !is_active || aimpos.is_zero( ) )
	{
		vel = {};
		locked_target = {};
		return;
	}

	vector3d angle_to = shoot_pos.angle_to( aimpos );
	angle_to.sanity_clamp( );
	angle_to.x -= punch.x;
	angle_to.y -= punch.y;
	angle_to.sanity_clamp( );

	vector3d angle = d.view_angles - angle_to;
	angle.sanity_clamp( );

	float smoothing_x = settings.smooth_enabled ? settings.smooth_x : 1.3f;
	float smoothing_y = settings.smooth_enabled ? settings.smooth_y : 1.3f;

	vector2d smoothing_backup = { smoothing_x, smoothing_y };

	if ( settings.random_smooth_enabled )
	{
		const int rx = ( int ) std::clamp( settings.randomize_smooth_x, 0.f, smoothing_x - 1.f );
		const int ry = ( int ) std::clamp( settings.randomize_smooth_y, 0.f, smoothing_y - 1.f );

		smoothing_x += ( float ) random( -rx, rx );
		smoothing_y += ( float ) random( -ry, ry );

		if ( smoothing_x < 1.f )
			smoothing_x = smoothing_backup.x;

		if ( smoothing_y < 1.f )
			smoothing_y = smoothing_backup.y;
	}

	if ( settings.smoothing_type == 1 )
	{
		smoothing_x = math::remap_val_clamped( fabsf( angle.x ), 0.f, settings.max_fov, smoothing_x, smoothing_x * 0.25f );
		smoothing_y = math::remap_val_clamped( fabsf( angle.y ), 0.f, settings.max_fov, smoothing_y, smoothing_y * 0.25f );
	}
	else if ( settings.smoothing_type == 2 )
	{
		smoothing_x = math::remap_val_clamped( fabsf( angle.x ), 0.f, settings.max_fov, smoothing_x * 0.25f, smoothing_x );
		smoothing_y = math::remap_val_clamped( fabsf( angle.y ), 0.f, settings.max_fov, smoothing_y * 0.25f, smoothing_y );
	}

	aim_at_safe(angle, nullptr, smoothing_x, smoothing_y);   // ← pass nullptr
}

void c_aimbot::aim_at_safe(const vector3d& angle,
	vector2d* overshoot_vec,
	const float& smoothing_x_local,
	const float& smoothing_y_local)
{
	vector3d delta = angle;

	normalize_da_angle(delta.y);
	delta.x = std::clamp(delta.x, -89.f, 89.f);
	delta.y = std::clamp(delta.y, -179.0f, 179.0f);

	// Exact same stepping as your original safe code
	if (delta.length_2d() >= 30.f)
	{
		if (delta.normalize_in_place() == 0.f)
			return;

		delta.x *= 28.f;
		delta.y *= 28.f;
		delta.z *= 28.f;
	}

	vector2d pixels = vector2d::from_3d(delta);
	float dist_px = std::hypot(pixels.x, pixels.y);

	if (dist_px < 1.5f)
		return;

	// Simple smoothed mouse movement (exactly like your old move_mouse)
	float move_x = pixels.x / smoothing_x_local;
	float move_y = pixels.y / smoothing_y_local;

	mouse_event(MOUSEEVENTF_MOVE,
		static_cast<long>(std::lround(-move_x)),
		static_cast<long>(std::lround(-move_y)),
		0, 0);
}