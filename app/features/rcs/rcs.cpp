#include "rcs.h"
#include "..\entity_cache\entity_cache.h"
#include "..\mouse\mouse.h"
#include "..\..\renderer\window_handler.h"

// this is already compensated
vector3d c_rcs::get_punch( bool change_from_cfg )
{
	auto& local = entity_cache->get_local_player();
	c_utl_vector<vector3d> aim_punch_cache = memory->read<c_utl_vector<vector3d>>((uintptr_t)(local.pawn) + 0x16F0);
	if ( !aim_punch_cache.m_size || !aim_punch_cache.m_data )
		return {};

	vector3d aim_punch = memory->read<vector3d>(reinterpret_cast<uintptr_t>(aim_punch_cache.m_data + (aim_punch_cache.m_size - 1)/* * sizeof( vector3d )*/));
	
	float cfg_x = 2.f * ( compensate_x / 100.f );
	float cfg_y = 2.f * ( compensate_y / 100.f );

	if ( !change_from_cfg )
		cfg_x = cfg_y = 2.f;

	return vector3d( aim_punch.x * cfg_x, aim_punch.y * cfg_y, 0.f );
}

void c_rcs::setup_weapon_config( )
{
	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.is_valid( ) )
		return;

	auto weapon_service = local_player.pawn->weapon_services( );
	if ( !weapon_service )
		return;

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( weapon_service->active_weapon_handle( ) );
	if ( !weapon )
		return;

	auto vdata = weapon->weapon_vdata( );
	if ( !vdata )
		return;

	int item_def_index = weapon->item_definition_index( );

	enabled = config_variables->rcs.global_weapon.enabled->value;
	compensate_x = config_variables->rcs.global_weapon.compensate_x->value;
	compensate_y = config_variables->rcs.global_weapon.compensate_y->value;
	smooth_x = config_variables->rcs.global_weapon.smooth_x->value;
	smooth_y = config_variables->rcs.global_weapon.smooth_y->value;

	switch ( weapon->weapon_vdata( )->weapon_type( ) )
	{
	case 1:

		if ( vdata->revolver( ) || item_def_index == 1 ) // deagle
		{
			if ( config_variables->rcs.heavy_pistols.override_global->value )
			{
				if ( config_variables->rcs.heavy_pistols.override_global->value )
				{
					enabled = config_variables->rcs.heavy_pistols.enabled->value;
					compensate_x = config_variables->rcs.heavy_pistols.compensate_x->value;
					compensate_y = config_variables->rcs.heavy_pistols.compensate_y->value;
					smooth_x = config_variables->rcs.heavy_pistols.smooth_x->value;
					smooth_y = config_variables->rcs.heavy_pistols.smooth_y->value;
					break;
				}
			}
		}
		else
		{
			if ( config_variables->rcs.pistols.override_global->value )
			{
				enabled = config_variables->rcs.pistols.enabled->value;
				compensate_x = config_variables->rcs.pistols.compensate_x->value;
				compensate_y = config_variables->rcs.pistols.compensate_y->value;
				smooth_x = config_variables->rcs.pistols.smooth_x->value;
				smooth_y = config_variables->rcs.pistols.smooth_y->value;
				break;
			}
		}
	case 2:
		if ( config_variables->rcs.smgs.override_global->value )
		{
			enabled = config_variables->rcs.smgs.enabled->value;
			compensate_x = config_variables->rcs.smgs.compensate_x->value;
			compensate_y = config_variables->rcs.smgs.compensate_y->value;
			smooth_x = config_variables->rcs.smgs.smooth_x->value;
			smooth_y = config_variables->rcs.smgs.smooth_y->value;
			break;
		}
	case 3:
		if ( config_variables->rcs.rifles.override_global->value )
		{
			enabled = config_variables->rcs.rifles.enabled->value;
			compensate_x = config_variables->rcs.rifles.compensate_x->value;
			compensate_y = config_variables->rcs.rifles.compensate_y->value;
			smooth_x = config_variables->rcs.rifles.smooth_x->value;
			smooth_y = config_variables->rcs.rifles.smooth_y->value;
			break;
		}
	case 6:
	case 4:
		if ( config_variables->rcs.heavy_rifles.override_global->value )
		{
			enabled = config_variables->rcs.heavy_rifles.enabled->value;
			compensate_x = config_variables->rcs.heavy_rifles.compensate_x->value;
			compensate_y = config_variables->rcs.heavy_rifles.compensate_y->value;
			smooth_x = config_variables->rcs.heavy_rifles.smooth_x->value;
			smooth_y = config_variables->rcs.heavy_rifles.smooth_y->value;
			break;
		}
	case 5:
		if ( config_variables->rcs.snipers.override_global->value )
		{
			enabled = config_variables->rcs.snipers.enabled->value;
			compensate_x = config_variables->rcs.snipers.compensate_x->value;
			compensate_y = config_variables->rcs.snipers.compensate_y->value;
			smooth_x = config_variables->rcs.snipers.smooth_x->value;
			smooth_y = config_variables->rcs.snipers.smooth_y->value;
			break;
		}
	}
}

void c_rcs::on_entity_update( const float& t )
{
	auto& local = entity_cache->get_local_player( );
	static vector3d last_punch = {};

	if ( !local.is_valid( ) )
	{
		last_punch = {};
		return;
	}

	setup_weapon_config( );
	if ( !enabled )
	{
		last_punch = {};
		return;
	}

	vector3d view_angles = local.pawn->eye_angles( );

	int shots_fired = local.pawn->shots_fired( );

	auto weapon_service = local.pawn->weapon_services( );
	if ( !weapon_service )
	{
		last_punch = {};
		return;
	}

	auto weapon = ( c_base_weapon* ) entity_helper->get_from_handle( weapon_service->active_weapon_handle( ) );
	if ( !weapon->weapon_vdata( ) )
	{
		last_punch = {};
		return;
	}

	if ( weapon->weapon_vdata( )->melee_weapon( ) )
	{
		last_punch = {};
		return;
	}


	/*if ( !( local.pawn->movement_services( )->buttons( ) & IN_ATTACK ) )
	{
		last_punch = {};
		return;
	}*/

	//if ( config_variables->rcs.rifle_only && !weapon->weapon_vdata( )->full_auto( ) )
	//	return;

	vector3d aim_punch = get_punch( true );
	if ( aim_punch.is_zero( ) )
		return;

	vector3d punch_angle = ( aim_punch - last_punch );
	punch_angle.sanity_clamp( );

	vector2d punch_pixels = vector2d::from_3d( punch_angle );
	punch_pixels.x /= smooth_x;
	punch_pixels.y /= smooth_y;

	mouse_accumulator->add_movement( -punch_pixels.x, -punch_pixels.y );
	last_punch = aim_punch;

}