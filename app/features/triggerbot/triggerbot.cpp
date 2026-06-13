#include "triggerbot.h"
#include "../menu/input/input.h"
#include "../../../valve/trace/trace.h"
#include "..\menu\menu.h"
#include "..\rcs\rcs.h"
#include "..\smoke\smoke.h"
#include <numbers>
uint32_t seed = 1337;

uint32_t lcg( )
{
	seed = 1664525 * seed + 1013904223;
	return seed;
}

int random( int min, int max )
{
	return min + ( lcg( ) % ( max - min + 1 ) );
}

void c_triggerbot::on_render( )
{
}

void c_triggerbot::setup_weapon_config( )
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

	const auto apply_config = [ & ] ( const c_config_variables::c_triggerbot::c_weapon_config_base& cfg )
		{
			delay = cfg.delay->value;
			dynamic_delay = cfg.dynamic_delay->value;
			dynamic_delay_value = cfg.dynamic_delay_value->value;
			static_delay_value = cfg.static_delay_value->value;
			next_attack_delay_value = cfg.next_attack_delay->value;
			hitchance = cfg.hitchance->value;
			hitchance_accuracy = cfg.hitchance_accuracy->value;
			autowall_enabled = cfg.autowall->value;
			autowall_mindamage = cfg.mindamage->value;
			head = cfg.head->value;
			chest = cfg.chest->value;
			pelvis = cfg.pelvis->value;
			legs = cfg.legs->value;
			toes = cfg.toes->value;
		};

	apply_config( config_variables->triggerbot.global_weapon );
	smoke_check = config_variables->triggerbot.smoke_check->value;

	const int item_def_index = weapon->item_definition_index( );

	switch ( vdata->weapon_type( ) )
	{
	case 1:
		if ( vdata->revolver( ) || item_def_index == 1 )
		{
			if ( config_variables->triggerbot.heavy_pistols.override_global->value )
			{
				apply_config( config_variables->triggerbot.heavy_pistols ); break;
			}
		}
		else
		{
			if ( config_variables->triggerbot.pistols.override_global->value )
			{
				apply_config( config_variables->triggerbot.pistols ); break;
			}
		}
	case 2:
		if ( config_variables->triggerbot.smgs.override_global->value )
		{
			apply_config( config_variables->triggerbot.smgs ); break;
		}
	case 3:
		if ( config_variables->triggerbot.rifles.override_global->value )
		{
			apply_config( config_variables->triggerbot.rifles ); break;
		}
	case 6:
	case 4:
		if ( config_variables->triggerbot.heavy_rifles.override_global->value )
		{
			apply_config( config_variables->triggerbot.heavy_rifles ); break;
		}
	case 5:
		if ( config_variables->triggerbot.snipers.override_global->value )
		{
			apply_config( config_variables->triggerbot.snipers ); break;
		}
	}
}

void c_triggerbot::on_input( )
{
	if ( !config_variables->triggerbot.enabled->value )
	{
		is_active = config_variables->triggerbot.trigger_key->active = false;
		return;
	}

	switch ( config_variables->triggerbot.trigger_key->activation_type )
	{
	case 0:
		is_active = config_variables->triggerbot.trigger_key->active = true;
		break;
	case 1:
	{
		auto s = input_handler->keybind_state( config_variables->triggerbot.trigger_key->key );
		is_active = config_variables->triggerbot.trigger_key->active = ( s == PRESSED || s == HELD );
		break;
	}
	case 2:
		if ( input_handler->keybind_state( config_variables->triggerbot.trigger_key->key ) == PRESSED )
			config_variables->triggerbot.trigger_key->active = !config_variables->triggerbot.trigger_key->active;
		is_active = config_variables->triggerbot.trigger_key->active;
		break;
	}

	setup_weapon_config( );
}

bool c_triggerbot::is_hitbox_enabled( int hitbox )
{
	if ( hitbox == HITBOX_HEAD || hitbox == HITBOX_NECK )
		return head;

	if ( hitbox == HITBOX_PELVIS )
		return pelvis;

	if ( hitbox == HITBOX_STOMACH || hitbox == HITBOX_THORAX ||
		hitbox == HITBOX_UPPER_CHEST || hitbox == HITBOX_CHEST )
		return chest;

	if ( hitbox == HITBOX_LEFT_THIGH || hitbox == HITBOX_RIGHT_THIGH )
		return legs;

	if ( hitbox == HITBOX_LEFT_FOOT || hitbox == HITBOX_RIGHT_FOOT )
		return legs;

	return false;
}

bool c_triggerbot::verify_last_target( const vector3d& eye_pos, const vector3d& end )
{
	if ( !last_best_player.is_valid( ) )
	{
		last_best_player = {};
		return false;
	}

	if ( !last_best_player.pawn->is_alive( ) )
	{
		last_best_player = {};
		return false;
	}

	vector3d mins = last_best_player.pawn->as_ent( )->collision( )->mins( );
	vector3d maxs = last_best_player.pawn->as_ent( )->collision( )->maxs( );
	mins += last_best_player.game_scene_node->origin( );
	maxs += last_best_player.game_scene_node->origin( );

	trace_t tr = {};
	ray_t ray = {};
	ray.init( eye_pos, end, last_best_player.pawn );

	if ( autowall_enabled )
	{
		if ( !trace->does_ray_hit_player( ray, &tr, mins, maxs, autowall_input ) )
		{
			last_best_player = {};
			return false;
		}
	}
	else if ( !trace->does_ray_hit_player( ray, &tr, mins, maxs, false ) )
	{
		last_best_player = {};
		return false;
	}

	if ( !is_hitbox_enabled( tr.m_hitbox_hit_index ) )
	{
		last_best_player = {};
		return false;
	}

	return true;
}

void c_triggerbot::on_entity_update( const float& t )
{
	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.is_valid( ) )
		return;

	if ( menu->is_open )
		return;

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( local_player.weapon_service->active_weapon_handle( ) );
	if ( !weapon )
		return;

	if ( weapon->clip_1( ) <= 0 )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata )
		return;

	if ( vdata->melee_weapon( ) )
		return;

	int type = vdata->weapon_type( );
	if ( type == 9 || type == 11 )
		return;

	static float next_activation_time = 0.f;
	static bool mouse_is_down = false;
	static float mouse_down_time {};
	static auto min_click_duration = 10 + random( -3, 3 );

	if ( mouse_is_down && t >= mouse_down_time )
	{
		if ( !vdata->revolver( ) )
			mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );

		mouse_is_down = false;
		min_click_duration = ( 10 + random( -3, 3 ) ); // metodka typu bypass hypixel autoclick anticheat
		return;
	}

	// KURWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	static auto off_view_angles = cs2_dumper::offsets::client_dll::dwViewAngles;
	static auto off_tickbase = schema_system->map_offsets[ FNV1A::HashConst( "CBasePlayerController->m_nTickBase" ) ];
	static auto off_recoil_index = schema_system->map_offsets[ FNV1A::HashConst( "C_CSWeaponBase->m_flRecoilIndex" ) ];
	static auto off_item_def = schema_system->map_offsets[ FNV1A::HashConst( "C_EconItemView->m_iItemDefinitionIndex" ) ];
	static auto off_num_bullets = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_nNumBullets" ) ];
	static auto off_spread = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flSpread" ) ];
	static auto off_range = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flRange" ) ];
	static auto off_clip1 = schema_system->map_offsets[ FNV1A::HashConst( "C_BasePlayerWeapon->m_iClip1" ) ];

	struct
	{
		vector3d view_angles = {};
		int tickbase = 0;
		float recoil_index = 0.f;
		int item_def_idx = 0;
		int num_bullets = 0;
		float spread = 0.f;
		float range = 0.f;
		int clip1 = 0;
	} d;

	{
		c_read_batch batch;
		batch.queue<vector3d>( sdk->client_dll( ).base + off_view_angles, &d.view_angles )
			.queue<int>( reinterpret_cast< uintptr_t >( local_player.controller ) + off_tickbase, &d.tickbase )
			.queue<float>( reinterpret_cast< uintptr_t >( weapon ) + off_recoil_index, &d.recoil_index )
			.queue<int>( reinterpret_cast< uintptr_t >( weapon ) + off_item_def, &d.item_def_idx )
			.queue<int>( reinterpret_cast< uintptr_t >( vdata ) + off_num_bullets, &d.num_bullets )
			.queue<float>( reinterpret_cast< uintptr_t >( vdata ) + off_spread, &d.spread )
			.queue<float>( reinterpret_cast< uintptr_t >( vdata ) + off_range, &d.range )
			.queue<int>( reinterpret_cast< uintptr_t >( weapon ) + off_clip1, &d.clip1 );
		memory->flush_batch( batch );
	}

	if ( autowall_enabled )
	{
		autowall_input.min_damage = autowall_mindamage;
		autowall_input.init( weapon );
	}

	vector3d eye_pos = local_player.pawn->get_eye_position( );
	vector3d punch = rcs->get_punch( false );
	vector3d fwd = {};
	math::angle_vectors( d.view_angles + punch, fwd );
	fwd *= 8192.f;
	vector3d end = eye_pos + fwd;

	bool is_last_target_valid = verify_last_target( eye_pos, end );

	if ( vdata->revolver( ) )
	{
		has_shot = ammo_at_start != d.clip1;
		if ( has_shot )
		{
			mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
			ammo_at_start = d.clip1;
			mouse_is_down = false;
			return;
		}
	}

	if ( !is_active )
		return;

	static const std::unordered_set<int> ghetto_fix = { 59, 43, 45, 46, 44, 47, 262175 };
	if ( ghetto_fix.contains( d.item_def_idx ) )
		return;

	int local_team = local_player.pawn->team_num( );

	//local_player.controller->cmd_angles( );
	//auto sequence_number = local_player.controller->sequence( );

	const float inaccuracy = weapon->get_inaccuracy( local_player.pawn, vdata );
	const uint32_t server_seed = math::get_server_seed( d.view_angles, d.tickbase + 1 );

	vector3d forward {}, right {}, up {};
	math::angle_vectors( d.view_angles, &forward, &right, &up );

	const auto set_target = [ & ] ( const c_cached_entity& player )
		{
			last_best_player = player;
			float d_delay = 0.f;
			if ( delay )
			{
				d_delay = static_delay_value;
				if ( dynamic_delay )
					d_delay += std::clamp( static_cast< float >( random( -dynamic_delay_value, dynamic_delay_value ) ), 0.f, d_delay );
			}
			shot_should_happen_at = t + d_delay;
			has_shot = false;
			ammo_at_start = d.clip1;
		};

	static auto off_globals = cs2_dumper::offsets::client_dll::dwGlobalVars;
	uintptr_t g_globals = memory->read<uintptr_t>( sdk->client_dll( ).base + off_globals );
	int tickcount = d.tickbase + 1;

	const auto check_spread_hits_seeded = [ & ] ( const c_cached_entity& player ) -> bool
		{
			vector3d mins = player.pawn->as_ent( )->collision( )->mins( );
			vector3d maxs = player.pawn->as_ent( )->collision( )->maxs( );
			mins += player.game_scene_node->origin( );
			maxs += player.game_scene_node->origin( );

			auto tick_hits = [ & ] ( int tick_offset ) -> bool
				{
					const uint32_t tick_seed = math::get_server_seed( d.view_angles, tickcount + tick_offset );
					vector2d sv = math::calc_spread( tick_seed + 1, inaccuracy, d.spread, d.recoil_index, d.item_def_idx, d.num_bullets );
					vector3d direction = {};

					direction = forward + ( right * sv.x ) + ( up * sv.y );
					direction.normalize_in_place( );

					ray_t ray {};
					trace_t tr {};
					ray.init( eye_pos, eye_pos + direction * d.range, player.pawn, direction );
					if ( autowall_enabled )
					{
						autowall_input.scale_damage( player.pawn, -1 );
						if ( trace->does_ray_hit_player( ray, &tr, mins, maxs, autowall_input )
							&& is_hitbox_enabled( tr.m_hitbox_hit_index ) )
							return true;
					}
					else if ( trace->does_ray_hit_player( ray, &tr, mins, maxs, false )
						&& is_hitbox_enabled( tr.m_hitbox_hit_index ) )
						return true;


					return false;
				};

			if ( tick_hits( 0 ) )
				return true;

			return false;
		};

	std::vector<c_cached_entity> players = entity_cache->get_players( );
	bool found_this_tick = false;

	if ( !is_last_target_valid )
	{
		for ( const auto& player : players )
		{
			if ( !player.is_valid( ) )
				continue;

			if ( !global_vars->team_cvar( ) && player.pawn->team_num( ) == local_team )
				continue;

			if ( !player.pawn->is_alive( ) )
				continue;

			if ( smoke_check )
			{
				vector3d player_origin = player.game_scene_node->origin( );
				if ( smoke_system->line_through_smoke( eye_pos, player_origin ) )
					continue;
			}

			if ( hitchance >= 100 )
			{
				if ( !check_spread_hits_seeded( player ) )
					continue;

				set_target( player );
				found_this_tick = true;
				break;
			}
			else
			{
				vector3d mins = player.pawn->as_ent( )->collision( )->mins( );
				vector3d maxs = player.pawn->as_ent( )->collision( )->maxs( );
				mins += player.game_scene_node->origin( );
				maxs += player.game_scene_node->origin( );

				trace_t tr = {};
				ray_t ray = {};
				ray.init( eye_pos, end, player.pawn );

				if ( autowall_enabled )
				{
					autowall_input.scale_damage( player.pawn, -1 );
					if ( !trace->does_ray_hit_player( ray, &tr, mins, maxs, autowall_input ) )
						continue;
				}
				else if ( !trace->does_ray_hit_player( ray, &tr, mins, maxs, false ) )
					continue;

				if ( !is_hitbox_enabled( tr.m_hitbox_hit_index ) )
					continue;

				set_target( player );
				break;
			}
		}
	}

	if ( !last_best_player.is_valid( ) )
		return;

	if ( smoke_check )
	{
		vector3d player_origin = last_best_player.game_scene_node->origin( );
		if ( smoke_system->line_through_smoke( eye_pos, player_origin ) )
		{
			last_best_player = {};
			return;
		}
	}

	if ( hitchance >= 100 )
	{
		// if we just found the target this tick its already good lol
		// only re-verify targets carried over from a run
		if ( !found_this_tick && !check_spread_hits_seeded( last_best_player ) )
			return;
	}
	else if ( !trace->hitchance( last_best_player.pawn, eye_pos, d.view_angles, d.range,
		inaccuracy, d.spread, d.recoil_index, d.item_def_idx, d.num_bullets,
		hitchance, hitchance_accuracy, autowall_enabled, server_seed ) )
		return;

	if ( t >= shot_should_happen_at && !mouse_is_down )
	{
		mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
		mouse_down_time = t + min_click_duration / 100.f;
		mouse_is_down = true;
		next_activation_time = t + next_attack_delay_value;
	}
}
