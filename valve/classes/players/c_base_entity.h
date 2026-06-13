#pragma once
#include "game_scene_node.h"

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK 0x7FFF
#define NUM_SERIAL_NUM_SHIFT_BITS 15
#define ENT_MAX_NETWORKED_ENTRY 16384

class c_base_handle
{
public:
	c_base_handle() :
		nIndex(INVALID_EHANDLE_INDEX) {
	}

	c_base_handle(const int nEntry, const int nSerial)
	{
		if (nEntry < 0 || (nEntry & ENT_ENTRY_MASK) != nEntry)
		{
			return;
		}
		if (nSerial < 0 || nSerial >= (1 << NUM_SERIAL_NUM_SHIFT_BITS))
		{
			return;
		}

		nIndex = nEntry | (nSerial << NUM_SERIAL_NUM_SHIFT_BITS);
	}
	bool is_valid()
	{
		return nIndex != INVALID_EHANDLE_INDEX;
	}

	int get_entry_index()
	{
		return static_cast<int>(nIndex & ENT_ENTRY_MASK);
	}

private:
	std::uint32_t nIndex {};
};

class c_player_pawn;
class c_player_controller;

class c_entity_instance {
public:
	std::string get_schema_name();
};

class c_collision_property
{
public:
	SCHEMA( vector3d, mins, "CCollisionProperty->m_vecMins" );
	SCHEMA( vector3d, maxs, "CCollisionProperty->m_vecMaxs" );
	SCHEMA( vector3d, surrounding_maxs, "CCollisionProperty->m_vecSurroundingMaxs" );
	SCHEMA( vector3d, surrounding_mins, "CCollisionProperty->m_vecSurroundingMins" );
};

class c_entity_sub_class_v_data_base
{
public:

};

enum class e_entity_type : uint8_t
{
	unknown,
	player,
	smoke,
	he,
	molotov,
	incendiary,
	flashbang,
	weapon,
	bomb,
	planted_bomb,
	kit,
	inferno,
};

class c_base_cs_grenade
{
public:
	SCHEMA( bool, is_held, "C_BaseCSGrenade->m_bIsHeldByPlayer" );
};

class c_base_entity : public c_entity_instance
{
public:
	SCHEMA(c_base_handle, get_owner_handle, "C_BaseEntity->m_hOwnerEntity");
	SCHEMA(c_base_handle, ground_entity_handle, "C_BaseEntity->m_hGroundEntity");
	SCHEMA(vector3d, view_offset, "C_BaseModelEntity->m_vecViewOffset");
	SCHEMA( c_collision_property*, collision, "C_BaseEntity->m_pCollision");
	SCHEMA( uint8_t, flags, "C_BaseEntity->m_fFlags");
	SCHEMA( c_game_scene_node*, game_scene_node, "C_BaseEntity->m_pGameSceneNode" );

	c_entity_sub_class_v_data_base* get_vdatabase( )
	{
		OFFSET_FROM( subclass_id, "C_BaseEntity->m_nSubclassID" );
		return memory->read<c_entity_sub_class_v_data_base*>( ( uintptr_t ) this + subclass_id + 0x8 );
	}

	c_player_pawn* pawn() 
	{
		return ( c_player_pawn* ) this;
	}

	c_player_controller* controller() 
	{
		return ( c_player_controller* ) this;
	}

	c_base_cs_grenade* grenade( )
	{
		return ( c_base_cs_grenade* ) this;
	}

	e_entity_type get_entity_type( ) // @ internal final boss
	{
		switch ( get_schema_hash( ) )
		{
		case HASH( "CCSPlayerController" ):				return e_entity_type::player;
		case HASH( "C_CSPlayerPawn" ):					return e_entity_type::player;
		case HASH( "C_SmokeGrenadeProjectile" ):		return e_entity_type::smoke;
		case HASH( "C_HEGrenadeProjectile" ):			return e_entity_type::he;
		case HASH( "C_MolotovProjectile" ):				return e_entity_type::molotov;
		case HASH( "C_IncendiaryGrenade" ):				return e_entity_type::incendiary;
		case HASH( "C_FlashbangProjectile" ):			return e_entity_type::flashbang;
		case HASH( "C_CSWeaponBaseGun" ):				return e_entity_type::weapon; // ??
		case HASH( "C_C4" ):				            return e_entity_type::bomb;
		case HASH( "C_PlantedC4" ):				        return e_entity_type::planted_bomb;
		case HASH( "CBaseAnimGraph" ):				    return e_entity_type::kit;
		case HASH( "C_Inferno" ):					    return e_entity_type::inferno;
		default:                                        return e_entity_type::unknown;
		}
	}

	uint32_t get_schema_hash( ) const
	{
		const auto entity_identity = memory->read<std::uintptr_t>( ( uintptr_t ) this + 0x10 );
		if ( !entity_identity )
			return 0;

		const auto entity_class_info = memory->read<std::uintptr_t>( entity_identity + 0x8 );
		if ( !entity_class_info )
			return 0;

		const auto schema_name_ptr = memory->read<std::uintptr_t>( entity_class_info + 0x8 );
		if ( !schema_name_ptr )
			return 0;

		const auto schema_name = memory->read<std::uintptr_t>( schema_name_ptr );
		if ( !schema_name )
			return 0;

		char class_name[ 64 ] {};
		memory->read_raw( schema_name, class_name, sizeof( class_name ) );

		if ( !class_name[ 0 ] )
			return 0;

		return FNV1A::Hash( class_name );
	}
};

class c_smoke_grenade_projectile : public c_base_entity
{
public:
	SCHEMA( int,      smoke_effect_tick_begin, "C_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin" );
	SCHEMA( bool,     did_smoke_effect,        "C_SmokeGrenadeProjectile->m_bDidSmokeEffect" );
	SCHEMA( vector3d, smoke_detonation_pos,    "C_SmokeGrenadeProjectile->m_vSmokeDetonationPos" );

	uintptr_t voxel_state_ptr( )
	{
		OFFSET_FROM( voxel_data_off, "C_SmokeGrenadeProjectile->m_VoxelFrameData" );
		const uintptr_t off = voxel_data_off ? voxel_data_off : 0x14A8;
		return memory->read<uintptr_t>( reinterpret_cast<uintptr_t>( this ) + off + 0x70 );
	}
};

