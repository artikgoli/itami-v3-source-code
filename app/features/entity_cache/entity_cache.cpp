#include "entity_cache.h"
#include "..\aimbot\aimbot.h"
#include "..\..\renderer\renderer.h"
#include "..\menu\input\input.h"

#include "..\rcs\rcs.h"
#include "..\mouse\mouse.h"
#include "../damage/damage.h"
#include "..\triggerbot\triggerbot.h"
#include "..\misc\misc.h"
#include "..\misc\nade helper\nade_helper.h"
#include "..\esp\esp.h"

void c_entity_cache::cache_hitboxes( c_player_pawn* pawn )
{
	if (!pawn || pawn == local_player.pawn)
		return;

	if (pawn->life_state() != 256)
		return;

	c_game_scene_node* scene_node = pawn->game_scene_node();
	if (!scene_node)
		return;

	uintptr_t model_handle = scene_node->skeleton_instance()->model_state().get_model_handle();
	if (!model_handle)
		return;

	uintptr_t model_strong = memory->read<uintptr_t>(model_handle);
	if (!model_strong)
		return;

	c_render_mesh* render_mesh = memory->read<c_render_mesh*>(memory->read<uintptr_t>(model_strong + 0x78));
	if (!render_mesh)
		return;

	c_hitbox_set* hitbox_set = render_mesh->get_hitbox_set();
	if (!hitbox_set)
		return;

	int hitbox_count = hitbox_set->get_hitbox_count();
	if (hitbox_count <= 0 || hitbox_count > 28) // 28 = max bone count
		return;

	uintptr_t hitbox_array_obj = hitbox_set->get_hitbox_array();
	if (!hitbox_array_obj)
		return;

	std::vector<cached_hitbox> cached_hitboxes;
	cached_hitboxes.reserve( hitbox_count );

	for ( int i = 0; i < hitbox_count; i++ )
	{
		auto hitbox = ( c_hitbox* )( hitbox_array_obj + i * 0x70 );
		cached_hitboxes.push_back( { i, hitbox->get_mins(), hitbox->get_maxs(), hitbox->get_radius(), hitbox->get_group_index(), hitbox->get_hitbox_name() } );
	}

	pawn_hitboxes[ pawn ].swap( cached_hitboxes );
}

// https://www.youtube.com/watch?v=7O6Gnl13tlw
void c_entity_cache::update( const float& global_time )
{
	float last_entity_update = last_entity_cache_update;
	if ( global_time - last_entity_update > 1.5f )
	{
		if ( !players.empty( ) )
			players.clear( );

		if ( !entities.empty( ) )
			entities.clear( );

		int highest_index = entity_helper->highest_entity_idx( );
		players.reserve( 66 );
		entities.reserve( ( size_t ) highest_index + 1 );
		local_player.index = -1;

		static auto off_game_scene_node = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pGameSceneNode" ) ];
		static auto off_weapon_services = schema_system->map_offsets[ FNV1A::HashConst( "C_BasePlayerPawn->m_pWeaponServices" ) ];

		{
			c_read_batch batch;
			batch.queue<c_player_controller*>( sdk->local_controller.address, &local_player.controller )
				.queue<c_player_pawn*>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn, &local_player.pawn );
			memory->flush_batch( batch );
		}
		{
			c_read_batch batch;
			const auto pawn_base = reinterpret_cast< uintptr_t >( local_player.pawn );
			batch.queue<c_game_scene_node*>( pawn_base + off_game_scene_node, &local_player.game_scene_node )
				.queue<c_weapon_services*>( pawn_base + off_weapon_services, &local_player.weapon_service );
			memory->flush_batch( batch );
		}
		local_player.entity = reinterpret_cast< c_base_entity* >( local_player.pawn );

		for ( auto& e : build_pending_entities( highest_index ) )
		{
			if ( ( e.controller == local_player.controller && local_player.controller ) ||
				( e.pawn == local_player.pawn && local_player.pawn ) )
				continue;

			c_cached_entity object {};
			object.entity = e.entity;
			object.index = e.index;
			object.controller = e.controller;
			object.pawn = e.pawn;
			object.game_scene_node = e.game_scene_node;
			object.team_num = e.team_num;

			if ( e.index <= 65 )
				players.emplace_back( object );
			else
				entities.emplace_back( object );
		}

		last_entity_cache_update = global_time + 1.5f;
	}

	float last_hitbox_cache = last_hitbox_cache_update;
	if ( global_time - last_hitbox_cache > 5.f )
	{
		if ( !players.empty( ) )
		{
			for ( auto& player : players )
				cache_hitboxes( player.pawn );

			last_hitbox_cache_update = global_time + 5.f;
		}
	}

	if ( !input_handler->poll_text_input( global_time ) )
	{
		input_handler->update_keybinds( );
		input_handler->poll_keybinding( global_time );
	}

	{
		damage->on_input( );
		aimbot->on_input( );
		triggerbot->on_input( );
		esp->on_input( );
		nade_helper->on_input( );

		damage->on_entity_update( global_time );
		rcs->on_entity_update( global_time );
		aimbot->on_entity_update( global_time );
		triggerbot->on_entity_update( global_time );
		misc->on_entity_update( global_time );
		nade_helper->on_entity_update( global_time );
		mouse_accumulator->apply_movement( );
	}
	input_handler->update_keybind_state( );
}

int c_entity_helper::highest_entity_idx( )
{
	uintptr_t ent_list = memory->read<uintptr_t>( sdk.get( )->client_dll( ).base + cs2_dumper::offsets::client_dll::dwEntityList );
	int highest_index = 0;

	uintptr_t block_ptrs[ 32 ] = {};
	{
		c_read_batch batch;
		for ( int block = 0; block < 32; ++block )
			batch.queue<uintptr_t>( ent_list + 0x10 + block * 0x8, &block_ptrs[ block ] );
		memory->flush_batch( batch );
	}

	thread_local uintptr_t slots[ 32 ][ 512 ];
	memset( slots, 0, sizeof( slots ) );
	{
		c_read_batch batch;
		for ( int block = 0; block < 32; ++block )
		{
			if ( !block_ptrs[ block ] ) continue;
			for ( int i = 0; i < 512; ++i )
				batch.queue<uintptr_t>( block_ptrs[ block ] + 0x70 * i, &slots[ block ][ i ] );
		}
		if ( !batch.empty( ) )
			memory->flush_batch( batch );
	}

	for ( int block = 31; block >= 0; --block )
		for ( int i = 511; i >= 0; --i )
			if ( slots[ block ][ i ] )
			{
				highest_index = block * 512 + i;
				goto done;
			}
done:
	return highest_index;
}

c_base_entity* c_entity_helper::get_from_handle( c_base_handle handle )
{
	if ( !handle.is_valid( ) )
		return nullptr;

	auto entry_index = handle.get_entry_index( );
	uintptr_t ent_list = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwEntityList );
	std::uintptr_t list_entry = memory->read<std::uint64_t>( ent_list + 0x8 * ( ( entry_index & 0x7FFF ) >> 9 ) + 0x10 );

	if ( !list_entry )
		return nullptr;

	return memory->read<c_base_entity*>( list_entry + 0x70 * ( entry_index & 0x1FF ) );
}

template<typename T>
T* c_entity_helper::get_from_index( int index )
{
	uintptr_t ent_list = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwEntityList );

	std::uintptr_t list_entry = memory->read<std::uint64_t>( ent_list + ( 0x8 * ( ( index & 0x7FFF ) >> 0x9 ) ) + 0x10 );
	if ( !list_entry )
		return nullptr;

	return memory->read<T*>( list_entry + 0x70 * ( index & 0x1FF ) );
}

bool c_entity_cache::is_fresh_game_tick( )
{
	auto global_vars = memory->read<std::uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwGlobalVars );
	
	static int old_tickcount = 0;
	// tickcount either 0x44 or 0x48
	// curtime 0x4C
	// sanitized map name eg. "de_mirage" 0x188
	int tickcount = memory->read<int>( global_vars + 0x44 );
	if ( tickcount != old_tickcount )
	{
		old_tickcount = tickcount;
		return true;
	}

	return false;
}

void c_entity_helper::batch_get_from_handles( std::vector<c_base_handle> handles, std::vector<c_base_entity*>& out_entities )
{
	out_entities.assign( handles.size( ), nullptr );
	uintptr_t ent_list = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwEntityList );
	if ( !ent_list )
		return;
	std::vector<std::uintptr_t> list_entries( handles.size( ), 0 );
	{
		c_read_batch batch;
		for ( size_t i = 0; i < handles.size( ); ++i )
		{
			if ( !handles[ i ].is_valid( ) )
				continue;

			auto entry_index = handles[ i ].get_entry_index( );
			batch.queue<std::uint64_t>( ent_list + 0x8 * ( ( entry_index & 0x7FFF ) >> 9 ) + 0x10, &list_entries[ i ] );
		}
		memory->flush_batch( batch );
	}
	{
		c_read_batch batch;
		for ( size_t i = 0; i < handles.size( ); ++i )
		{
			if ( !handles[ i ].is_valid( ) || !list_entries[ i ] )
				continue;

			auto entry_index = handles[ i ].get_entry_index( );
			batch.queue<c_base_entity*>( list_entries[ i ] + 0x70 * ( entry_index & 0x1FF ), &out_entities[ i ] );
		}
		memory->flush_batch( batch );
	}
}

std::vector<c_entity_cache::pending_entity_t> c_entity_cache::build_pending_entities( int highest_index )
{
	static auto off_game_scene_node = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pGameSceneNode" ) ];
	static auto off_team_num = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_iTeamNum" ) ];
	static auto off_pawn_handle = schema_system->map_offsets[ FNV1A::HashConst( "CBasePlayerController->m_hPawn" ) ];

	std::vector<pending_entity_t> pending;
	pending.reserve( highest_index );

	for ( int i = 1; i <= highest_index; ++i )
	{
		c_base_entity* entity = entity_helper->get_from_index( i );
		if ( !entity )
			continue;
		auto& e = pending.emplace_back( );
		e.entity = entity;
		e.index = i;
		e.controller = reinterpret_cast< c_player_controller* >( entity );
	}

	if ( pending.empty( ) )
		return pending;

	std::vector<c_base_handle> pawn_handles( pending.size( ) );
	{
		c_read_batch batch;
		for ( size_t i = 0; i < pending.size( ); ++i )
			batch.queue<c_base_handle>( reinterpret_cast< uintptr_t >( pending[ i ].controller ) + off_pawn_handle, &pawn_handles[ i ] );
		memory->flush_batch( batch );
	}

	std::vector<c_base_entity*> resolved_pawns;
	entity_helper->batch_get_from_handles( pawn_handles, resolved_pawns );
	for ( size_t i = 0; i < pending.size( ); ++i )
		pending[ i ].pawn = reinterpret_cast< c_player_pawn* >( resolved_pawns[ i ] );

	{
		c_read_batch batch;
		for ( auto& e : pending )
		{
			if ( !e.pawn )
				continue;
			const auto pawn_base = reinterpret_cast< uintptr_t >( e.pawn );
			batch.queue<c_game_scene_node*>( pawn_base + off_game_scene_node, &e.game_scene_node )
				.queue<int>( pawn_base + off_team_num, &e.team_num );
		}
		memory->flush_batch( batch );
	}

	return pending;
}