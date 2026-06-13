#include "esp.h"
#include "..\entity_cache\entity_cache.h"
#include "..\..\renderer\renderer.h"
#include "../../renderer/window_handler.h"
#include "..\menu\input\input.h"

static void cache_players_stub( )
{
	esp->cache_players( );
}
static void cache_entities_stub( )
{
	esp->cache_entities( );
}

void c_esp::init( )
{
	std::thread( cache_players_stub ).detach( );
	std::thread( cache_entities_stub ).detach( );
}

bool c_esp::calc_bbox( const c_cached_entity& ent, rect_t& out )
{
	float left = 99999.f, top = 99999.f, right = -99999.f, bottom = -99999.f;

	auto project = [ & ] ( vector3d world ) -> bool
		{
			vector2d screen;
			if ( !render->world_to_screen( world, screen ) )
				return false;
			left = min( left, screen.x );
			top = min( top, screen.y );
			right = max( right, screen.x );
			bottom = max( bottom, screen.y );
			return true;
		};

	const vector3d& mn = ent.mins;
	const vector3d& mx = ent.maxs;

	if ( ent.use_transform )
	{
		const matrix4x4 trs = matrix4x4::from_trs( ent.transform );
		const vector3d corners[ 8 ] =
		{
			{ mn.x, mn.y, mn.z }, { mn.x, mx.y, mn.z },
			{ mx.x, mx.y, mn.z }, { mx.x, mn.y, mn.z },
			{ mn.x, mn.y, mx.z }, { mn.x, mx.y, mx.z },
			{ mx.x, mx.y, mx.z }, { mx.x, mn.y, mx.z }
		};

		for ( auto& c : corners )
			if ( !project( trs.transform_point( c ) ) )
				return false;
	}
	else
	{
		const vector3d corners[ 8 ] =
		{
			{ mn.x, mn.y, mn.z }, { mn.x, mx.y, mn.z },
			{ mx.x, mx.y, mn.z }, { mx.x, mn.y, mn.z },
			{ mx.x, mx.y, mx.z }, { mn.x, mx.y, mx.z },
			{ mn.x, mn.y, mx.z }, { mx.x, mn.y, mx.z }
		};

		for ( auto& c : corners )
			if ( !project( c + ent.origin ) )
				return false;
	}

	out.x = left;
	out.y = top;
	out.w = right - left;
	out.h = bottom - top;
	return true;
}

void c_esp::cache_players( )
{
	static auto off_pawn_handle = schema_system->map_offsets[ FNV1A::HashConst( "CBasePlayerController->m_hPawn" ) ];
	static auto off_owner = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_hOwnerEntity" ) ];
	static auto off_life_state = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_lifeState" ) ];
	static auto off_health = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_iHealth" ) ];
	static auto off_max_health = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_iMaxHealth" ) ];
	static auto off_team = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_iTeamNum" ) ];
	static auto off_armor = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_ArmorValue" ) ];
	static auto off_flash = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawnBase->m_flFlashDuration" ) ];
	static auto off_scoped = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_bIsScoped" ) ];
	static auto off_defusing = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_bIsDefusing" ) ];
	static auto off_immune = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_bGunGameImmunity" ) ];
	static auto off_skel_inst = schema_system->map_offsets[ FNV1A::HashConst( "CBodyComponentSkeletonInstance->m_skeletonInstance" ) ];
	static auto off_active_weapon = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_pClippingWeapon" ) ];
	static auto off_clip1     = schema_system->map_offsets[ FNV1A::HashConst( "C_BasePlayerWeapon->m_iClip1" ) ];
	static auto off_reloading = schema_system->map_offsets[ FNV1A::HashConst( "C_CSWeaponBase->m_bInReload" ) ];
	static auto off_collision = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pCollision" ) ];
	static auto off_item_svcs = schema_system->map_offsets[ FNV1A::HashConst( "C_BasePlayerPawn->m_pItemServices" ) ];
	static auto off_gsn_origin = schema_system->map_offsets[ FNV1A::HashConst( "CGameSceneNode->m_vecOrigin" ) ];
	static auto off_col_maxs = schema_system->map_offsets[ FNV1A::HashConst( "CCollisionProperty->m_vecMaxs" ) ];
	static auto off_has_helmet = schema_system->map_offsets[ FNV1A::HashConst( "CCSPlayer_ItemServices->m_bHasHelmet" ) ];
	static auto off_has_defuser = schema_system->map_offsets[ FNV1A::HashConst( "CCSPlayer_ItemServices->m_bHasDefuser" ) ];

	struct pending_t
	{
		c_base_entity* entity = nullptr;
		c_player_controller* controller = nullptr;
		c_player_pawn* pawn = nullptr;
		c_game_scene_node* gsn = nullptr;
		uintptr_t bone_array = 0;
		int life_state = 0, health = 0, max_health = 0, team = 0, armor = 0;
		float flash = 0.f;
		bool scoped = false, defusing = false, immune = false;
		int entity_index = 0;
		c_base_weapon* weapon = nullptr;
		int clip_1 = -1;
		bool reloading = false;
		c_collision_property* collision_ptr = nullptr;
		c_player_item_services* item_svcs = nullptr;
		vector3d origin = {};
		vector3d maxs_val = {};
		bool helmet = false;
		bool kit = false;
	};

	while ( true )
	{
		c_player_controller* local_controller {};
		uintptr_t c4_handle {};
		{
			c_read_batch batch;
			batch.queue<c_player_controller*>( sdk->local_controller.address, &local_controller )
				.queue<uintptr_t>( sdk->client_dll( ).add( cs2_dumper::offsets::client_dll::dwWeaponC4 ), &c4_handle );
			memory->flush_batch( batch );
		}

		if ( !local_controller )
		{
			player_buf_size[ 1 - player_front.load( ) ] = 0;
			player_front.store( 1 - player_front.load( ) );
			Sleep( 5 );
			continue;
		}

		c_player_pawn* bomb_owner = nullptr;
		if ( c4_handle )
		{
			c_base_entity* c4 = memory->read<c_base_entity*>( c4_handle );
			if ( c4 )
			{
				c_base_handle h = memory->read<c_base_handle>( reinterpret_cast< uintptr_t >( c4 ) + off_owner );
				if ( h.is_valid( ) )
					bomb_owner = reinterpret_cast< c_player_pawn* >( entity_helper->get_from_handle( h ) );
			}
		}

		std::vector<pending_t> pending;
		pending.reserve( 65 );

		for ( int i = 1; i <= 65; ++i )
		{
			c_base_entity* entity = entity_helper->get_from_index( i );
			if ( !entity )
				continue;
			auto& e = pending.emplace_back( );
			e.entity = entity;
			e.controller = reinterpret_cast< c_player_controller* >( entity );
			e.entity_index = i;
		}

		if ( pending.empty( ) )
		{
			int back = 1 - player_front.load( );
			player_buf_size[ back ] = 0;
			player_front.store( back );
			Sleep( 5 );
			continue;
		}

		pending.erase(
			std::remove_if( pending.begin( ), pending.end( ),
				[ & ] ( const auto& e )
				{
					return e.controller == local_controller;
				} ),
			pending.end( ) );

		std::vector<c_base_handle> pawn_handles( pending.size( ) );
		{
			c_read_batch batch;
			for ( size_t i = 0; i < pending.size( ); ++i )
				batch.queue<c_base_handle>( reinterpret_cast< uintptr_t >( pending[ i ].controller ) + off_pawn_handle, &pawn_handles[ i ] );
			memory->flush_batch( batch );
		}

		static std::vector<c_base_entity*> resolved_pawns;
		entity_helper->batch_get_from_handles( pawn_handles, resolved_pawns );
		for ( size_t i = 0; i < pending.size( ); ++i )
			pending[ i ].pawn = reinterpret_cast< c_player_pawn* >( resolved_pawns[ i ] );

		pending.erase(
			std::remove_if( pending.begin( ), pending.end( ), [ ] ( const auto& e )
				{
					return !e.pawn;
				} ),
			pending.end( ) );

		{
			c_read_batch batch;
			for ( auto& e : pending )
			{
				const auto b = reinterpret_cast< uintptr_t >( e.pawn );
				batch.queue<int>( b + off_life_state, &e.life_state )
					.queue<int>( b + off_health, &e.health )
					.queue<int>( b + off_max_health, &e.max_health )
					.queue<int>( b + off_team, &e.team )
					.queue<int>( b + off_armor, &e.armor )
					.queue<float>( b + off_flash, &e.flash )
					.queue<bool>( b + off_scoped, &e.scoped )
					.queue<bool>( b + off_defusing, &e.defusing )
					.queue<bool>( b + off_immune, &e.immune )
					.queue<c_game_scene_node*>( b + schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pGameSceneNode" ) ], &e.gsn )
					.queue<c_base_weapon*>( b + off_active_weapon, &e.weapon )
					.queue<c_collision_property*>( b + off_collision, &e.collision_ptr )
					.queue<c_player_item_services*>( b + off_item_svcs, &e.item_svcs );
			}
			memory->flush_batch( batch );
		}

		pending.erase(
			std::remove_if( pending.begin( ), pending.end( ),
				[ ] ( const auto& e )
				{
					return e.life_state != 256 || e.health <= 0 || !e.gsn;
				} ),
			pending.end( ) );

		if ( pending.empty( ) )
		{
			int back = 1 - player_front.load( );
			player_buf_size[ back ] = 0;
			player_front.store( back );
			Sleep( 5 );
			continue;
		}

		static auto off_model_state = schema_system->map_offsets[ FNV1A::HashConst( "CSkeletonInstance->m_modelState" ) ];

		{
			c_read_batch batch;
			for ( auto& e : pending )
				batch.queue<uintptr_t>( reinterpret_cast< uintptr_t >( e.gsn ) + off_model_state + off_skel_inst, &e.bone_array );

			memory->flush_batch( batch );
		}

		static std::vector<std::array<c_bone_data, 28>> all_bones;
		all_bones.resize( pending.size( ) );
		{
			c_read_batch batch;
			for ( size_t i = 0; i < pending.size( ); ++i )
			{
				if ( !pending[ i ].bone_array )
					continue;

				for ( int b = 0; b < 28; ++b )
					batch.queue<c_bone_data>( pending[ i ].bone_array + b * sizeof( c_bone_data ), &all_bones[ i ][ b ] );
			}
			memory->flush_batch( batch );
		}

		{
			c_read_batch batch;
			for ( auto& e : pending )
			{
				if ( e.weapon )
				{
					const auto base = reinterpret_cast< uintptr_t >( e.weapon );
					batch.queue<int>( base + off_clip1,     &e.clip_1 )
						.queue<bool>( base + off_reloading, &e.reloading );
				}

				if ( e.gsn )
					batch.queue<vector3d>( reinterpret_cast< uintptr_t >( e.gsn ) + off_gsn_origin, &e.origin );

				if ( e.collision_ptr )
					batch.queue<vector3d>( reinterpret_cast< uintptr_t >( e.collision_ptr ) + off_col_maxs, &e.maxs_val );

				if ( e.item_svcs )
				{
					batch.queue<bool>( reinterpret_cast< uintptr_t >( e.item_svcs ) + off_has_helmet, &e.helmet );
					batch.queue<bool>( reinterpret_cast< uintptr_t >( e.item_svcs ) + off_has_defuser, &e.kit );
				}
			}
			memory->flush_batch( batch );
		}

		int back = 1 - player_front.load( );
		unsigned int local_size = 0;
		vector3d local_eye = entity_cache->get_local_player( ).pawn->get_eye_position( );

		bool defusing_flag = config_variables->esp.defuse_flag->value;
		bool zoomed_flag = config_variables->esp.zoom_flag->value;
		bool flashed_flag = config_variables->esp.flash_flag->value;
		bool armor_flag = config_variables->esp.armor_flag->value;
		bool immune_flag = config_variables->esp.immune_flag->value;
		bool bomb_flag = config_variables->esp.bomb_flag->value;

		for ( size_t pi = 0; pi < pending.size( ); ++pi )
		{
			auto& e = pending[ pi ];
			if ( local_size >= 65 )
				break;

			c_cached_player& out = player_buf[ back ][ local_size++ ];
			out = c_cached_player {};

			out.maxs = e.maxs_val;
			out.mins = { -out.maxs.x, -out.maxs.y, 0.f };
			out.name = e.controller->get_name( );
			std::replace( out.name.begin( ), out.name.end( ), '\r', ' ' );
			out.name_hash = std::hash<std::string>{}( out.name );
			out.origin = e.origin;
			out.health = e.health;
			out.max_health = e.max_health;
			out.team = e.team;
			out.armor = ( armor_flag ? e.armor : 0 );
			out.blind = e.flash > 0.f && flashed_flag;
			out.scoped = e.scoped && zoomed_flag;
			out.defusing = e.defusing && defusing_flag;
			out.damage_immune = e.immune && immune_flag;
			out.helmet = e.helmet && armor_flag;
			out.kit = e.kit;
			out.has_bomb = ( e.pawn == bomb_owner ) && bomb_flag;
			out.is_player = true;
			out.entity_index = e.entity_index;
			out.is_visible = (
				parser->is_visible( local_eye, out.origin ) ||
				parser->is_visible( local_eye, out.origin + vector3d( 0, 0, out.maxs.z * 0.5f ) ) ||
				parser->is_visible( local_eye, out.origin + vector3d( 0, 0, out.maxs.z ) ) );

			for ( int b = 0; b < 28; ++b )
			{
				out.bone_pos[ b ] = all_bones[ pi ][ b ].origin;
				out.bone_rot[ b ] = all_bones[ pi ][ b ].rotation;
			}
			out.pawn = e.pawn;

			if ( e.weapon )
			{
				auto vdata = e.weapon->weapon_vdata( );
				if ( vdata )
				{
					auto wname = vdata->name( );
					if ( wname.size( ) > 7 )
						wname.erase( 0, 7 );

					out.weapon_name = std::move( wname );
					out.weapon_name_hash = std::hash<std::string>{}( out.weapon_name );
					out.weapon_name_fnv = FNV1A::Hash( out.weapon_name.c_str( ) );
					out.max_clip = vdata->max_clip_1( );
				}
				out.clip_1 = e.clip_1;
				out.reloading = e.reloading;
			}
		}

		player_buf_size[ back ] = local_size;
		player_front.store( back );

		Sleep( 5 );
	}
}

void c_esp::cache_entities( )
{
	static auto off_owner = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_hOwnerEntity" ) ];
	static auto off_explode_began = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseCSGrenadeProjectile->m_bExplodeEffectBegan" ) ];
	static auto off_smoke_tick = schema_system->map_offsets[ FNV1A::HashConst( "C_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin" ) ];
	static auto off_fire_tick = schema_system->map_offsets[ FNV1A::HashConst( "C_Inferno->m_nFireEffectTickBegin" ) ];
	static auto off_fire_lifetime = schema_system->map_offsets[ FNV1A::HashConst( "C_Inferno->m_nFireLifetime" ) ];
	static auto off_ent_gsn = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pGameSceneNode" ) ];
	static auto off_ent_col = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_pCollision" ) ];
	static auto off_transform = schema_system->map_offsets[ FNV1A::HashConst( "CGameSceneNode->m_nodeToWorld" ) ];
	static auto off_gsn_origin = schema_system->map_offsets[ FNV1A::HashConst( "CGameSceneNode->m_vecOrigin" ) ];
	static auto off_col_mins = schema_system->map_offsets[ FNV1A::HashConst( "CCollisionProperty->m_vecMins" ) ];
	static auto off_col_maxs = schema_system->map_offsets[ FNV1A::HashConst( "CCollisionProperty->m_vecMaxs" ) ];

	static std::unordered_map<uintptr_t, e_entity_type> schema_ptr_to_type;

	static std::vector<std::uint64_t> list_entries;
	static std::vector<c_base_entity*> all_entities;
	static std::vector<uintptr_t> identities;
	static std::vector<uintptr_t> class_infos;
	static std::vector<uintptr_t> name_ptrs;
	static std::vector<uintptr_t> schema_name_ptrs;

	struct pending_t
	{
		c_base_entity* entity = nullptr;
		c_base_handle owner_handle = {};
		e_entity_type type = e_entity_type::unknown;
		bool has_owner = false;
		int entity_index = 0;
		bool exploded = false;
		int smoke_tick = 0;
		int fire_tick = 0;
		float fire_lifetime = 0.f;
		c_game_scene_node* gsn = nullptr;
		c_collision_property* col = nullptr;
		transform_4d transform {};
		vector3d origin {};
		vector3d mins {};
		vector3d maxs {};
	};

	struct class_name_buf
	{
		char data[ 64 ] {};
	};

	while ( true )
	{
		int highest_index = entity_helper->highest_entity_idx( );
		if ( highest_index <= 16 )
		{
			int back = 1 - entity_front.load( );
			entity_buf_size[ back ] = 0;
			entity_front.store( back );
			Sleep( 5 );
			continue;
		}

		uintptr_t ent_list = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwEntityList );
		if ( !ent_list )
		{
			int back = 1 - entity_front.load( );
			entity_buf_size[ back ] = 0;
			entity_front.store( back );
			Sleep( 5 );
			continue;
		}

		const int max_chunks = ( ( highest_index & 0x7FFF ) >> 9 ) + 1;
		list_entries.assign( max_chunks, 0 );
		{
			c_read_batch batch;
			for ( int chunk = 0; chunk < max_chunks; ++chunk )
				batch.queue<std::uint64_t>( ent_list + 0x8 * chunk + 0x10, &list_entries[ chunk ] );
			memory->flush_batch( batch );
		}

		all_entities.assign( highest_index + 1, nullptr );
		{
			c_read_batch batch;
			for ( int i = 16; i <= highest_index; ++i )
			{
				int chunk = ( i & 0x7FFF ) >> 9;
				if ( !list_entries[ chunk ] )
					continue;
				batch.queue<c_base_entity*>( list_entries[ chunk ] + 0x70 * ( i & 0x1FF ), &all_entities[ i ] );
			}
			memory->flush_batch( batch );
		}

		identities.assign( highest_index + 1, 0 );
		{
			c_read_batch batch;
			for ( int i = 16; i <= highest_index; ++i )
			{
				if ( !all_entities[ i ] )
					continue;
				batch.queue<uintptr_t>( reinterpret_cast< uintptr_t >( all_entities[ i ] ) + 0x10, &identities[ i ] );
			}
			memory->flush_batch( batch );
		}

		class_infos.assign( highest_index + 1, 0 );
		{
			c_read_batch batch;
			for ( int i = 16; i <= highest_index; ++i )
			{
				if ( !identities[ i ] )
					continue;
				batch.queue<uintptr_t>( identities[ i ] + 0x8, &class_infos[ i ] );
			}
			memory->flush_batch( batch );
		}

		name_ptrs.assign( highest_index + 1, 0 );
		{
			c_read_batch batch;
			for ( int i = 16; i <= highest_index; ++i )
			{
				if ( !class_infos[ i ] )
					continue;
				batch.queue<uintptr_t>( class_infos[ i ] + 0x8, &name_ptrs[ i ] );
			}
			memory->flush_batch( batch );
		}

		schema_name_ptrs.assign( highest_index + 1, 0 );
		{
			c_read_batch batch;
			for ( int i = 16; i <= highest_index; ++i )
			{
				if ( !name_ptrs[ i ] )
					continue;
				batch.queue<uintptr_t>( name_ptrs[ i ], &schema_name_ptrs[ i ] );
			}
			memory->flush_batch( batch );
		}

		{
			std::unordered_map<uintptr_t, class_name_buf> ptr_to_name;
			{
				c_read_batch batch;
				for ( int i = 16; i <= highest_index; ++i )
				{
					uintptr_t ptr = schema_name_ptrs[ i ];
					if ( !ptr || schema_ptr_to_type.count( ptr ) || ptr_to_name.count( ptr ) )
						continue;

					ptr_to_name[ ptr ] = {};
					batch.queue<class_name_buf>( ptr, &ptr_to_name[ ptr ] );
				}
				if ( !ptr_to_name.empty( ) )
					memory->flush_batch( batch );
			}

			for ( auto& [ptr, buf] : ptr_to_name )
			{
				if ( !buf.data[ 0 ] )
					continue;
				uint32_t hash = FNV1A::Hash( buf.data );
				e_entity_type type = e_entity_type::unknown;
				switch ( hash )
				{
				case FNV1A::HashConst( "CCSPlayerController" ):      type = e_entity_type::player;       break;
				case FNV1A::HashConst( "C_CSPlayerPawn" ):           type = e_entity_type::player;       break;
				case FNV1A::HashConst( "C_SmokeGrenadeProjectile" ): type = e_entity_type::smoke;        break;
				case FNV1A::HashConst( "C_HEGrenadeProjectile" ):    type = e_entity_type::he;           break;
				case FNV1A::HashConst( "C_MolotovProjectile" ):      type = e_entity_type::molotov;      break;
				case FNV1A::HashConst( "C_IncendiaryGrenade" ):      type = e_entity_type::incendiary;   break;
				case FNV1A::HashConst( "C_FlashbangProjectile" ):    type = e_entity_type::flashbang;    break;
				case FNV1A::HashConst( "C_CSWeaponBaseGun" ):        type = e_entity_type::weapon;       break;
				case FNV1A::HashConst( "C_C4" ):                     type = e_entity_type::bomb;         break;
				case FNV1A::HashConst( "C_PlantedC4" ):              type = e_entity_type::planted_bomb; break;
				case FNV1A::HashConst( "CBaseAnimGraph" ):           type = e_entity_type::kit;          break;
				case FNV1A::HashConst( "C_Inferno" ):                type = e_entity_type::inferno;      break;
				default: break;
				}
				schema_ptr_to_type[ ptr ] = type;
			}
		}

		std::vector<pending_t> pending;
		pending.reserve( 64 );

		for ( int i = 16; i <= highest_index; ++i )
		{
			if ( !all_entities[ i ] || !schema_name_ptrs[ i ] )
				continue;

			auto it = schema_ptr_to_type.find( schema_name_ptrs[ i ] );
			if ( it == schema_ptr_to_type.end( ) )
				continue;

			e_entity_type type = it->second;
			if ( type == e_entity_type::unknown || type == e_entity_type::player || type == e_entity_type::weapon )
				continue;

			auto& e = pending.emplace_back( );
			e.entity = all_entities[ i ];
			e.type = type;
			e.entity_index = i;
		}

		if ( pending.empty( ) )
		{
			int back = 1 - entity_front.load( );
			entity_buf_size[ back ] = 0;
			entity_front.store( back );
			Sleep( 5 );
			continue;
		}

		{
			std::vector<size_t> owner_indices;
			{
				c_read_batch batch;
				for ( size_t i = 0; i < pending.size( ); ++i )
				{
					auto t = pending[ i ].type;
					if ( t == e_entity_type::bomb || t == e_entity_type::planted_bomb || t == e_entity_type::kit )
					{
						batch.queue<c_base_handle>( reinterpret_cast< uintptr_t >( pending[ i ].entity ) + off_owner, &pending[ i ].owner_handle );
						owner_indices.push_back( i );
					}
				}
				if ( !owner_indices.empty( ) )
					memory->flush_batch( batch );
			}

			if ( !owner_indices.empty( ) )
			{
				std::vector<c_base_handle> owner_handles( owner_indices.size( ) );
				for ( size_t i = 0; i < owner_indices.size( ); ++i )
					owner_handles[ i ] = pending[ owner_indices[ i ] ].owner_handle;

				std::vector<c_base_entity*> owners;
				entity_helper->batch_get_from_handles( owner_handles, owners );

				for ( size_t i = 0; i < owner_indices.size( ); ++i )
					pending[ owner_indices[ i ] ].has_owner = owners[ i ] != nullptr;
			}
		}

		{
			c_read_batch batch;
			for ( auto& e : pending )
			{
				const auto b = reinterpret_cast< uintptr_t >( e.entity );
				if ( e.type == e_entity_type::smoke )
					batch.queue<int>( b + off_smoke_tick, &e.smoke_tick );
				else if ( e.type == e_entity_type::inferno )
					batch.queue<int>( b + off_fire_tick, &e.fire_tick )
					.queue<float>( b + off_fire_lifetime, &e.fire_lifetime );
				else if ( e.type == e_entity_type::he || e.type == e_entity_type::flashbang ||
					e.type == e_entity_type::molotov || e.type == e_entity_type::incendiary )
					batch.queue<bool>( b + off_explode_began, &e.exploded );
				batch.queue<c_game_scene_node*>( b + off_ent_gsn, &e.gsn );
				if ( e.type == e_entity_type::bomb || e.type == e_entity_type::kit )
					batch.queue<c_collision_property*>( b + off_ent_col, &e.col );
			}
			memory->flush_batch( batch );
		}

		pending.erase(
			std::remove_if( pending.begin( ), pending.end( ), [ ] ( const auto& e )
				{
					return ( e.type == e_entity_type::he || e.type == e_entity_type::flashbang ||
						e.type == e_entity_type::molotov || e.type == e_entity_type::incendiary )
						&& e.exploded;
				} ),
			pending.end( ) );

		{
			c_read_batch batch;
			for ( auto& e : pending )
			{
				if ( !e.gsn )
					continue;
				if ( e.type == e_entity_type::inferno || e.type == e_entity_type::he ||
					e.type == e_entity_type::flashbang || e.type == e_entity_type::smoke ||
					e.type == e_entity_type::molotov || e.type == e_entity_type::incendiary )
					batch.queue<vector3d>( reinterpret_cast< uintptr_t >( e.gsn ) + off_gsn_origin, &e.origin );
				else
					batch.queue<transform_4d>( reinterpret_cast< uintptr_t >( e.gsn ) + off_transform, &e.transform );
				if ( e.col && ( e.type == e_entity_type::bomb || e.type == e_entity_type::kit ) )
				{
					batch.queue<vector3d>( reinterpret_cast< uintptr_t >( e.col ) + off_col_mins, &e.mins );
					batch.queue<vector3d>( reinterpret_cast< uintptr_t >( e.col ) + off_col_maxs, &e.maxs );
				}
			}
			memory->flush_batch( batch );
		}

		int back = 1 - entity_front.load( );
		unsigned int local_size = 0;
		planted_bomb_idx.store( -1 );

		for ( auto& e : pending )
		{
			if ( !e.gsn )
				continue;

			if ( e.type == e_entity_type::planted_bomb )
				planted_bomb_idx.store( e.entity_index );

			const bool uses_transform = ( e.type == e_entity_type::planted_bomb ||
				e.type == e_entity_type::bomb || e.type == e_entity_type::kit );
			if ( !uses_transform && e.origin.is_zero( ) )
				continue;

			auto& entry = entity_buf[ back ][ local_size ];
			entry = c_cached_entity {};
			bool populated = false;

			if ( config_variables->esp.world_enabled->value )
			{
				if ( config_variables->esp.world_projectiles->value )
				{
					auto set_grenade = [ & ] ( const char* label )
						{
							entry.name = label;
							entry.mins = vector3d( -5, -5, -5 );
							entry.maxs = vector3d( 5, 5, 5 );
							entry.origin = e.origin;
							entry.use_transform = false;
							populated = true;
						};

					if ( e.type == e_entity_type::flashbang )
						set_grenade( "flashbang" );
					else if ( e.type == e_entity_type::he )
						set_grenade( "he" );
					else if ( e.type == e_entity_type::molotov || e.type == e_entity_type::incendiary )
						set_grenade( "fire" );
					else if ( e.type == e_entity_type::smoke )
					{
						set_grenade( "smoke" );
						if ( e.smoke_tick > 0 )
						{
							entry.type = e_entity_type::smoke;
							entry.timer_active = true;
							entry.timer_tick_begin = e.smoke_tick;
							entry.timer_duration = 18.f;
						}
					}

					if ( e.type == e_entity_type::inferno )
					{
						entry.name = "fire";
						entry.origin = e.origin;
						entry.type = e_entity_type::inferno;
						if ( e.fire_tick > 0 )
						{
							entry.timer_active = true;
							entry.timer_tick_begin = e.fire_tick;
							entry.timer_duration = ( e.fire_lifetime > 0.f ? e.fire_lifetime : 7.f ) - 0.7f;
						}
						populated = true;
					}
				}

				if ( config_variables->esp.world_bomb->value )
				{
					if ( e.type == e_entity_type::bomb && !e.has_owner && e.col )
					{
						entry.name = "afghanistan";
						entry.mins = e.mins;
						entry.maxs = e.maxs;
						entry.transform = e.transform;
						entry.use_transform = true;
						populated = true;
					}
					else if ( e.type == e_entity_type::planted_bomb )
					{
						entry.name = "afghanistan (ready)";
						entry.mins = vector3d( -5, -5, -5 );
						entry.maxs = vector3d( 5, 5, 5 );
						entry.transform = e.transform;
						entry.use_transform = true;
						populated = true;
					}
				}

				if ( config_variables->esp.world_kit->value && !e.has_owner && e.type == e_entity_type::kit && e.col )
				{
					entry.name = "defuse kit";
					entry.mins = e.mins;
					entry.maxs = e.maxs;
					entry.transform = e.transform;
					entry.use_transform = true;
					populated = true;
				}
			}

			if ( populated )
			{
				entry.is_player = false;
				entry.entity_index = e.entity_index;
				local_size++;
			}
		}

		entity_buf_size[ back ] = local_size;
		entity_front.store( back );

		Sleep( 5 );
	}
}

void c_esp::render_entities( )
{
	auto gvars = memory->read<uintptr_t>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwGlobalVars );
	int tickcount = memory->read<int>( gvars + 0x44 );

	using clock = std::chrono::steady_clock;
	using fsec = std::chrono::duration<float>;
	static int last_tick = 0;
	static float clock_offset = 0.f;
	float now = std::chrono::duration_cast< fsec >( clock::now( ).time_since_epoch( ) ).count( );
	if ( tickcount != last_tick )
	{
		clock_offset = tickcount * 0.015625f - now;
		last_tick = tickcount;
	}
	float curtime = now + clock_offset;

	int front = entity_front.load( );
	unsigned int snap_size = entity_buf_size[ front ];

	for ( unsigned int i = 0; i < snap_size; i++ )
	{
		const auto& ent = entity_buf[ front ][ i ];

		auto draw_timer = [ & ] ( float cx, float cy )
			{
				constexpr int bar_w = 50, bar_h = 3;
				int bx = ( int ) cx - bar_w / 2;
				int by = ( int ) cy;
				float sec = max( 0.f, ( 0.015625f * ent.timer_tick_begin + ent.timer_duration ) - curtime );
				float frac = ent.timer_duration > 0.f ? std::clamp( sec / ent.timer_duration, 0.f, 1.f ) : 0.f;
				color fill = ent.type == e_entity_type::smoke ? color( 200, 200, 220, 255 ) : color( 250, 120, 50, 255 );
				render->rect( bx - 1, by - 1, bar_w + 2, bar_h + 2, color( 0, 0, 0, 180 ), true );
				render->rect( bx, by, bar_w, bar_h, color( 40, 40, 40, 255 ), true );
				render->rect( bx, by, ( int ) ( bar_w * frac ), bar_h, fill, true );
				render->text( bx + bar_w / 2, by + bar_h + 2, draw_pool->font_esp, color( 255, 255, 255, 255 ),
					daisy::TEXT_ALIGNX_CENTER, true, "%.1fs", sec );
			};

		if ( ent.timer_active )
		{
			float sec = ( 0.015625f * ent.timer_tick_begin + ent.timer_duration ) - curtime;
			if ( sec <= 0.f )
				continue;
		}

		if ( ent.type == e_entity_type::inferno )
		{
			vector2d screen {};
			if ( render->world_to_screen( ent.origin, screen ) )
			{
				render->text( ( int ) screen.x, ( int ) screen.y - 14, draw_pool->font_esp,
					color( 255, 255, 255, 255 ), daisy::TEXT_ALIGNX_CENTER, true, ent.name.data( ) );
				if ( config_variables->esp.world_grenade_timers->value && ent.timer_active )
					draw_timer( screen.x, screen.y );
			}
			continue;
		}

		rect_t bbox {};
		if ( !calc_bbox( ent, bbox ) )
			continue;

		render->rect( bbox.x, bbox.y, bbox.w, bbox.h, color( 255, 255, 255, 255 ), false );
		render->rect( bbox.x - 1, bbox.y - 1, bbox.w + 2, bbox.h + 2, color( 0, 0, 0, 255 ), false );
		render->rect( bbox.x + 1, bbox.y + 1, bbox.w - 2, bbox.h - 2, color( 0, 0, 0, 255 ), false );
		render->text( bbox.x + bbox.w / 2, bbox.y - 14, draw_pool->font_esp, color( 255, 255, 255, 255 ),
			daisy::TEXT_ALIGNX_CENTER, true, ent.name.data( ) );

		if ( config_variables->esp.world_grenade_timers->value && ent.timer_active )
			draw_timer( bbox.x + bbox.w / 2.f, bbox.y + bbox.h + 4.f );
	}
}

static const char* weapon_display_name_map( const std::string& name )
{
	static const std::unordered_map<std::string, const char*> map = {
		{ "ak47",                 "AK-47"           },
		{ "aug",                  "AUG"             },
		{ "awp",                  "AWP"             },
		{ "bayonet",              "Bayonet"         },
		{ "bizon",                "PP-Bizon"        },
		{ "c4",                   "C4"              },
		{ "cz75a",                "CZ75-Auto"       },
		{ "deagle",               "Desert Eagle"    },
		{ "elite",                "Dual Berettas"   },
		{ "famas",                "FAMAS"           },
		{ "fiveseven",            "Five-SeveN"      },
		{ "decoy",                "Decoy"           },
		{ "flashbang",            "Flashbang"       },
		{ "hegrenade",            "HE Grenade"      },
		{ "incgrenade",           "Incendiary"      },
		{ "molotov",              "Molotov"         },
		{ "smokegrenade",         "Smoke"           },
		{ "g3sg1",                "G3SG1"           },
		{ "galilar",              "Galil AR"        },
		{ "glock",                "Glock-18"        },
		{ "healthshot",           "Healthshot"      },
		{ "hkp2000",              "P2000"           },
		{ "knife",                "Knife"           },
		{ "knife_t",              "Knife"           },
		{ "knife_survival_bowie", "Bowie Knife"     },
		{ "knife_butterfly",      "Butterfly Knife" },
		{ "knife_canis",          "Paracord Knife"  },
		{ "knife_gut",            "Gut Knife"       },
		{ "knife_falchion",       "Falchion Knife"  },
		{ "knife_flip",           "Flip Knife"      },
		{ "knife_css",            "Classic Knife"   },
		{ "knife_gypsy_jackknife","Navaja Knife"    },
		{ "knife_karambit",       "Karambit"        },
		{ "knife_kukri",          "Kukri Knife"     },
		{ "knife_m9_bayonet",     "M9 Bayonet"      },
		{ "knife_outdoor",        "Nomad Knife"     },
		{ "knife_push",           "Shadow Daggers"  },
		{ "knife_skeleton",       "Skeleton Knife"  },
		{ "knife_stiletto",       "Stiletto Knife"  },
		{ "knife_tactical",       "Huntsman Knife"  },
		{ "knife_ursus",          "Ursus Knife"     },
		{ "knife_widowmaker",     "Talon Knife"     },
		{ "m249",                 "M249"            },
		{ "m4a1",                 "M4A4"            },
		{ "m4a1_silencer",        "M4A1-S"          },
		{ "mac10",                "MAC-10"          },
		{ "mag7",                 "MAG-7"           },
		{ "mp5sd",                "MP5-SD"          },
		{ "mp7",                  "MP7"             },
		{ "mp9",                  "MP9"             },
		{ "negev",                "Negev"           },
		{ "nova",                 "Nova"            },
		{ "p90",                  "P90"             },
		{ "p250",                 "P250"            },
		{ "revolver",             "R8 Revolver"     },
		{ "sawedoff",             "Sawed-Off"       },
		{ "scar20",               "SCAR-20"         },
		{ "sg556",                "SG 553"          },
		{ "ssg08",                "SSG 08"          },
		{ "taser",                "Zeus x27"        },
		{ "tec9",                 "Tec-9"           },
		{ "ump45",                "UMP-45"          },
		{ "usp_silencer",         "USP-S"           },
		{ "xm1014",               "XM1014"          },
	};
	auto it = map.find( name );
	return it != map.end( ) ? it->second : nullptr;
}

void c_esp::render_players( )
{
	static float prev_time = 0.f;
	float cur_time = global_vars->get_time( );
	float frame_time = min( cur_time - prev_time, 0.1f );
	prev_time = cur_time;

	auto& local_player = entity_cache->get_local_player( );
	if ( !local_player.pawn )
	{
		animation_data.clear( );
		return;
	}

	int local_team = local_player.pawn->team_num( );
	vector3d local_origin = local_player.pawn->game_scene_node( )->origin( );

	for ( auto& [idx, data] : animation_data )
	{
		if ( data.is_valid )
			data.was_valid = true;

		data.is_valid = false;
	}

	int front = player_front.load( );
	unsigned int snap_size = player_buf_size[ front ];

	const bool team_mode = global_vars->team_cvar( );
	const vector3d oof_local_pos = entity_cache->get_local_player( ).game_scene_node->abs_origin( );
	const vector3d oof_view_angles = memory->read<vector3d>( sdk->client_dll( ).base + cs2_dumper::offsets::client_dll::dwViewAngles );

	bool enabled {};
	{
		std::lock_guard guard( is_enabled_mutex );
		enabled = is_enabled;
	}

	for ( unsigned int i = 0; i < snap_size; i++ )
	{
		const auto& entity = player_buf[ front ][ i ];

		if ( !config_variables->esp.players_enabled->value )
			continue;

		if ( !team_mode && entity.team == local_team )
			continue;

		auto [it, inserted] = animation_data.try_emplace( entity.entity_index );
		auto& anim = it->second;

		anim.origin   = entity.origin;
		anim.maxs     = entity.maxs;
		anim.bone_pos = entity.bone_pos;
		anim.bone_rot = entity.bone_rot;
		anim.pawn     = entity.pawn;
		anim.armor = entity.armor;
		anim.helmet = entity.helmet;
		anim.kit = entity.kit;
		anim.scoped = entity.scoped;
		anim.blind = entity.blind;
		anim.defusing = entity.defusing;
		anim.has_bomb = entity.has_bomb;
		anim.is_visible = entity.is_visible;
		anim.team = entity.team;

		if ( anim.hashed_name != entity.name_hash )
		{
			anim.name_w = global_vars->utf8_to_wide( entity.name );
			anim.name_w.erase( std::remove( anim.name_w.begin( ), anim.name_w.end( ), L'\r' ), anim.name_w.end( ) );
			anim.hashed_name = entity.name_hash;
		}

		if ( anim.hashed_weapon_name != entity.weapon_name_hash )
		{
			anim.hashed_weapon_name = entity.weapon_name_hash;
			const char* display = weapon_display_name_map( entity.weapon_name );
			anim.weapon_display_name = display ? display : entity.weapon_name;
		}
		anim.clip_1 = entity.clip_1;
		anim.max_clip = entity.max_clip;
		anim.reloading = entity.reloading;

		if ( inserted )
		{
			anim.health_displayed = static_cast< float >( entity.health );
			anim.health_target = static_cast< float >( entity.health );
			anim.health_start = static_cast< float >( entity.health );
			anim.health_anim_time = cur_time;
			anim.max_health = entity.max_health > 0 ? entity.max_health : 100;
			float init_ratio = ( entity.max_clip > 0 && entity.clip_1 >= 0 )
				? std::clamp( ( float ) entity.clip_1 / ( float ) entity.max_clip, 0.f, 1.f ) : 0.f;
			anim.ammo_displayed = init_ratio;
			anim.ammo_target    = init_ratio;
			anim.ammo_anim_time = cur_time;
			anim.global_alpha = 0.f;
		}
		else
		{
			anim.update_health( entity.health, entity.max_health, cur_time );
			anim.update_ammo( entity.clip_1, entity.max_clip, cur_time );
		}

		anim.is_valid = enabled;

		if ( config_variables->esp.player_oof->value )
			draw_oof( entity, oof_local_pos, oof_view_angles, cur_time );
	}

	for ( auto it = animation_data.begin( ); it != animation_data.end( ); )
	{
		auto& anim = it->second;

		if ( anim.was_valid && !anim.is_valid && anim.health_target != 0.f )
		{
			anim.health_start = anim.health_displayed;
			anim.health_anim_time = cur_time;
			anim.was_valid = false;
		}

		if ( anim.update_alpha( frame_time, cur_time ) )
		{
			it = animation_data.erase( it );
			continue;
		}

		anim.update_visibility( frame_time, config_variables->esp.players_invisible_alpha->value,
			config_variables->esp.players_invis_alpha_value->value / 100.f );

		float alpha = anim.global_alpha * anim.visibility_alpha;
		if ( alpha <= 0.f ) 
		{
			++it; 
			continue;
		}

		c_cached_entity bbox_ent {};
		bbox_ent.origin = anim.origin;
		bbox_ent.mins = { -anim.maxs.x, -anim.maxs.y, 0.f };
		bbox_ent.maxs = anim.maxs;

		rect_t box {};
		if ( !calc_bbox( bbox_ent, box ) ) 
		{
			++it; 
			continue;
		}

		if ( config_variables->esp.player_box->value )
		{
			auto box_clr = config_variables->esp.player_box_color->value;
			box_clr.a = ( byte ) ( box_clr.a * alpha );
			color outline( 0, 0, 0, ( byte ) ( 255 * alpha ) );
			render->rect( ( int ) box.x, ( int ) box.y, ( int ) box.w, ( int ) box.h, box_clr, false );
			render->rect( ( int ) box.x - 1, ( int ) box.y - 1, ( int ) box.w + 2, ( int ) box.h + 2, outline, false );
			render->rect( ( int ) box.x + 1, ( int ) box.y + 1, ( int ) box.w - 2, ( int ) box.h - 2, outline, false );
		}

		if ( config_variables->esp.player_name->value )
			render->text_w( box.x + box.w / 2, box.y - 15,
				draw_pool->font_esp, color( 255, 255, 255, ( byte ) ( 255 * alpha ) ),
				daisy::TEXT_ALIGNX_CENTER, true, anim.name_w.c_str( ) );

		if ( config_variables->esp.player_healthbar->value )
			draw_healthbar( anim, box, alpha );

		const float distance = ( anim.origin - local_origin ).length( );

		if ( config_variables->esp.player_weapon->value )
			draw_weapon( anim, box, alpha );

		if ( config_variables->esp.player_skeleton->value )
			draw_skeleton( anim, distance, alpha );

		if ( config_variables->esp.player_hitbox->value )
			draw_head_indicator( anim, alpha );

		draw_flags( anim, box, alpha );

		++it;
	}
}

void c_esp::draw_healthbar( const esp_animation_data& anim, const rect_t& box, float alpha )
{
	float ratio = std::clamp( anim.health_displayed / ( float ) anim.max_health, 0.f, 1.f );
	float bar_h = box.h * ratio;
	byte a = ( byte ) ( 255 * alpha );
	color fill( ( int ) ( ( 1.f - ratio ) * 255.f ), ( int ) ( ratio * 255.f ), 0, a );
	render->rect( box.x - 7, box.y - 1, 4, box.h + 3, color( 5, 5, 5, a ), true );
	render->gradient_rect( box.x - 6, box.y + ( box.h - bar_h ), 2, bar_h + 1, fill, fill, true );
}

void c_esp::draw_weapon( const esp_animation_data& anim, const rect_t& box, float alpha )
{
	if ( anim.weapon_display_name.empty( ) )
		return;

	auto wpn_color  = config_variables->esp.player_weapon_clr->value;
	auto ammo_color = config_variables->esp.player_weapon_ammo_clr->value;

	const byte a = ( byte ) ( 255 * alpha );
	const bool has_bar = anim.clip_1 >= 0 && anim.max_clip > 0 && config_variables->esp.player_weapon_ammo->value;
	const int base = ( int ) ( box.y + box.h );
	const int text_x = ( int ) ( box.x + box.w / 2 );

	int text_y;
	if ( has_bar )
	{
		render->rect( ( int ) box.x - 1, base + 3, ( int ) box.w + 3, 4, color( 5, 5, 5, a ), true );
		render->rect( ( int ) box.x,     base + 4, ( int ) ( ( box.w + 1 ) * anim.ammo_displayed ), 2,
			color( ammo_color.r, ammo_color.g, ammo_color.b, a ), true );
		text_y = base + 9;
	}
	else
	{
		text_y = base + max( 2, ( int ) ( box.h * 0.05f ) );
	}

	render->text( text_x, text_y, draw_pool->font_esp_pixel,
		color( wpn_color.r, wpn_color.g, wpn_color.b, a ),
		daisy::TEXT_ALIGNX_CENTER, true, anim.weapon_display_name.c_str( ) );
}

void c_esp::draw_oof( const c_cached_player& entity, const vector3d& local_pos, const vector3d& view_angles, float cur_time )
{
	const vector3d enemy_pos = entity.origin;

	if ( local_pos.is_zero( ) || enemy_pos.is_zero( ) )
		return;

	vector2d screen_pos;
	vector2d screen = window_handler->window_size;

	if ( render->world_to_screen( enemy_pos, screen_pos ) )
	{
		if ( screen_pos.x >= 0 && screen_pos.x <= screen.x &&
			screen_pos.y >= 0 && screen_pos.y <= screen.y )
			return;
	}

	vector3d delta = enemy_pos - local_pos;
	float final = deg_2_rad( view_angles.y - 90.f ) - std::atan2( delta.y, delta.x );
	float distance = static_cast< float >( local_pos.distance( enemy_pos ) * 0.0254f );

	if ( distance > config_variables->esp.player_oof_range->value )
		return;

	color clr = config_variables->esp.player_oof_color->value;
	if ( distance < 5.f )
		clr.a = ( byte ) ( clr.a * ( 0.4f + ( std::sin( cur_time * 8.f ) * 0.5f + 0.5f ) * 0.6f ) );

	render->arrow_aa( screen.x * 0.5f, screen.y * 0.5f, final,
		config_variables->esp.player_oof_radius->value,
		config_variables->esp.player_oof_size->value, clr, clr );
}

void c_esp::draw_skeleton( const esp_animation_data& anim, float distance, float alpha )
{
	const float scale = std::clamp( 700.f / distance, 0.35f, 1.0f );
	std::array<vector2d, 28> screen_pos {};
	std::bitset<28> visible;

	for ( int i = 0; i < 28; ++i )
	{
		if ( !anim.bone_pos[ i ].is_zero( ) && render->world_to_screen( anim.bone_pos[ i ], screen_pos[ i ] ) )
			visible.set( i );
	}

	static const std::array<std::pair<int, int>, 16> connections = { {
		{ bones_t::BONE_HEAD, bones_t::BONE_NECK },
		{ bones_t::BONE_NECK, bones_t::BONE_SPINE },
		{ bones_t::BONE_SPINE, bones_t::BONE_SPINE_1 },
		{ bones_t::BONE_SPINE_1, bones_t::BONE_HIP },
		{ bones_t::BONE_HIP, bones_t::BONE_LEFT_HIP },
		{ bones_t::BONE_HIP, bones_t::BONE_RIGHT_HIP },
		{ bones_t::BONE_NECK, bones_t::BONE_LEFT_SHOULDER },
		{ bones_t::BONE_LEFT_SHOULDER, bones_t::BONE_LEFT_ARM },
		{ bones_t::BONE_LEFT_ARM, bones_t::BONE_LEFT_HAND },
		{ bones_t::BONE_NECK, bones_t::BONE_RIGHT_SHOULDER },
		{ bones_t::BONE_RIGHT_SHOULDER, bones_t::BONE_RIGHT_ARM },
		{ bones_t::BONE_RIGHT_ARM, bones_t::BONE_RIGHT_HAND },
		{ bones_t::BONE_LEFT_HIP, bones_t::BONE_LEFT_KNEE },
		{ bones_t::BONE_LEFT_KNEE, bones_t::BONE_LEFT_FEET },
		{ bones_t::BONE_RIGHT_HIP, bones_t::BONE_RIGHT_KNEE },
		{ bones_t::BONE_RIGHT_KNEE, bones_t::BONE_RIGHT_FEET },
	} };

	auto skel_clr = config_variables->esp.player_skeleton_color->value;
	skel_clr.a = ( byte ) ( skel_clr.a * alpha );

	for ( const auto& [a, b] : connections )
	{
		if ( !visible.test( a ) || !visible.test( b ) )
			continue;
		render->line_aa( screen_pos[ a ].x, screen_pos[ a ].y,
			screen_pos[ b ].x, screen_pos[ b ].y, skel_clr, 1.4f * scale );
	}
}

void c_esp::draw_head_indicator( const esp_animation_data& anim, float alpha )
{
	if ( !anim.pawn )
		return;

	const auto it = entity_cache->pawn_hitboxes.find( anim.pawn );
	if ( it == entity_cache->pawn_hitboxes.end( ) )
		return;

	const c_entity_cache::cached_hitbox* hb = nullptr;
	for ( const auto& h : it->second )
	{
		if ( h.bone_index == HITBOX_HEAD ) 
		{ hb = &h; 
		break; }
	}

	if ( !hb || hb->radius < 0.f ) 
		return;

	const int bi = c_hitbox::get_bone_from_hitbox( HITBOX_HEAD );
	if ( bi < 0 || bi >= 28 ) 
		return;

	c_bone_data bd {};
	bd.origin   = anim.bone_pos[ bi ];
	bd.rotation = anim.bone_rot[ bi ];
	const matrix3x4_t mat = math::transform_to_matrix( &bd );

	const vector3d exp( hb->radius, hb->radius, hb->radius );
	const vector3d mn = hb->min - exp;
	const vector3d mx = hb->max + exp;

	const vector3d local[ 8 ] = {
		{ mn.x, mn.y, mn.z }, { mx.x, mn.y, mn.z },
		{ mx.x, mx.y, mn.z }, { mn.x, mx.y, mn.z },
		{ mn.x, mn.y, mx.z }, { mx.x, mn.y, mx.z },
		{ mx.x, mx.y, mx.z }, { mn.x, mx.y, mx.z },
	};

	vector2d sc[ 8 ];
	bool     ok[ 8 ];
	bool     any_on = false; // ale ghetto

	for ( int i = 0; i < 8; ++i )
	{
		ok[ i ] = render->world_to_screen( math::matrix_transform( mat, local[ i ] ), sc[ i ] );
		if ( ok[ i ] ) 
			any_on = true;
	}

	if ( !any_on ) 
		return;

	const auto& cfg = config_variables->esp.player_hitbox_color->value;
	const byte  raw  = ( byte ) ( cfg.a * alpha );
	const byte  rawf = ( byte ) ( raw * 0.4f );
	const color wire( cfg.r, cfg.g, cfg.b, raw );
	const color fill( cfg.r, cfg.g, cfg.b, rawf );
	const daisy::color_t dc_wire { ( uint8_t ) wire.r, ( uint8_t ) wire.g, ( uint8_t ) wire.b, ( uint8_t ) wire.a };
	const daisy::color_t dc_fill { ( uint8_t ) fill.r, ( uint8_t ) fill.g, ( uint8_t ) fill.b, ( uint8_t ) fill.a };

	static constexpr int k_edges[ 12 ][ 2 ] = {
		{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
		{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
	};
	static constexpr int k_faces[ 6 ][ 4 ] = {
		{ 0, 1, 2, 3 }, { 4, 5, 6, 7 },
		{ 0, 1, 5, 4 }, { 3, 2, 6, 7 },
		{ 0, 3, 7, 4 }, { 1, 2, 6, 5 },
	};

	auto pt = [ & ] ( int i ) -> daisy::point_t { 
		return { sc[ i ].x, sc[ i ].y }; };

	for ( const auto& f : k_faces )
	{
		if ( !ok[ f[ 0 ] ] || !ok[ f[ 1 ] ] || !ok[ f[ 2 ] ] || !ok[ f[ 3 ] ] ) 
			continue;

		draw_pool->buffer->queue.push_filled_triangle( pt( f[ 0 ] ), pt( f[ 1 ] ), pt( f[ 2 ] ), dc_fill, dc_fill, dc_fill );
		draw_pool->buffer->queue.push_filled_triangle( pt( f[ 0 ] ), pt( f[ 2 ] ), pt( f[ 3 ] ), dc_fill, dc_fill, dc_fill );
	}

	for ( const auto& e : k_edges )
	{
		if ( !ok[ e[ 0 ] ] || !ok[ e[ 1 ] ] ) 
			continue;

		render->line_aa( sc[ e[ 0 ] ].x, sc[ e[ 0 ] ].y, sc[ e[ 1 ] ].x, sc[ e[ 1 ] ].y, wire, 1.f );
	}
}

void c_esp::draw_flags( const esp_animation_data& anim, const rect_t& box, float alpha )
{
	int offset = 0;
	byte a = ( byte ) ( 255 * alpha );

	auto draw_flag = [ & ] ( const char* text, color clr )
		{
			clr.a = a;
			render->text( box.x + box.w + 4, box.y + offset, draw_pool->font_esp_s, clr, daisy::TEXT_ALIGNX_LEFT, true, text );
			offset += 10;
		};

	if ( anim.has_bomb )
		render->text( box.x + box.w / 2, box.y - 25, draw_pool->font_esp, color( 255, 80, 80, a ),
			daisy::TEXT_ALIGNX_CENTER, true, "JIHAD" );

	if ( anim.armor > 0 )
		draw_flag( anim.helmet ? "HK" : "K", color( 150, 150, 255 ) );

	if ( anim.kit )      draw_flag( "KIT", color( 102, 255, 255 ) );
	if ( anim.scoped )   draw_flag( "ZOOM", color( 102, 178, 255 ) );
	if ( anim.blind )    draw_flag( "FLASH", color( 255, 255, 102 ) );
	if ( anim.defusing ) draw_flag( "DEFUSE", color( 102, 255, 102 ) );
	//if ( anim.defusing ) draw_flag( "DEFUSE", color( 255, 255, 255 ) ); // immune flag?
}

void c_esp::on_input( )
{
	static bool keybind_toggle = false;
	auto keybind_key = config_variables->esp.player_esp_keybind->key;

	switch ( config_variables->esp.player_esp_keybind->activation_type )
	{
	case 0: keybind_toggle = true; break;
	case 1: 
	{
		auto s = input_handler->keybind_state( keybind_key );
		keybind_toggle = s == HELD || s == PRESSED;
		break;
	}
	case 2:
	{
		if ( input_handler->keybind_state( keybind_key ) == PRESSED )
			keybind_toggle = !keybind_toggle;

		break;
	}
	}

	config_variables->esp.player_esp_keybind->active = keybind_toggle;

	std::lock_guard guard( is_enabled_mutex );
	is_enabled = keybind_toggle && config_variables->esp.players_enabled;
}