#pragma once
#include "c_base_entity.h"

enum ItemDefinitionIndexes
{
	WEAPON_NONE,
	WEAPON_DESERTEAGLE,
	WEAPON_ELITES,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK18,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_ZONE_REPULSOR,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFE,
	WEAPON_KNIFE1,
	WEAPON_FLASHBANG,
	WEAPON_HE_GRENADE,
	WEAPON_SMOKE_GRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_KEVLAR,
	WEAPON_ASSAULTSUIT,
	WEAPON_HEAVYASSAULTSUIT,
	WEAPON_DEFUSER = 55,
	WEAPON_CUTTERS,
	WEAPON_HEALTHSHOT,
	TYPE_MUSICKIT,
	WEAPON_KNIFE_T,
	WEAPON_M4_SILENCER,
	WEAPON_USP_SILENCER,
	RECIPE_TRADEUP,
	WEAPON_CZ75,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_KNIFE2 = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_KNIFEBAYONET = 500,
	WEAPON_KNIFECSS = 503,
	WEAPON_KNIFEFLIP = 505,
	WEAPON_KNIFEGUT,
	WEAPON_KNIFEKARAM,
	WEAPON_KNIFEM9,
	WEAPON_KNIFETACTICAL,
	WEAPON_KNIFE_FALCHION_ADVANCED = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_CORD,
	WEAPON_KNIFE_CANIS,
	WEAPON_KNIFE_URSUS,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_OUTDOOR,
	WEAPON_KNIFE_STILETTO,
	WEAPON_KNIFE_WIDOWMAKER,
	WEAPON_KNIFE_SKELETON = 525
};

class c_weapon_base_vdata
{
public:
	SCHEMA( int, weapon_type, "CCSWeaponBaseVData->m_WeaponType" );
	SCHEMA( bool, full_auto, "CCSWeaponBaseVData->m_bIsFullAuto" );
	SCHEMA( bool, melee_weapon, "CCSWeaponBaseVData->m_bMeleeWeapon" );
	SCHEMA( bool, revolver, "CCSWeaponBaseVData->m_bIsRevolver" );
	SCHEMA( float, weapon_range, "CCSWeaponBaseVData->m_flRange" );
	SCHEMA( float, weapon_spread, "CCSWeaponBaseVData->m_flSpread" );
	SCHEMA( float, max_speed, "CCSWeaponBaseVData->m_flMaxSpeed" );
	SCHEMA( float, inacc_fire, "CCSWeaponBaseVData->m_flInaccuracyFire" );
	SCHEMA( float, inacc_move, "CCSWeaponBaseVData->m_flInaccuracyMove" );
	SCHEMA( float, jump_initial, "CCSWeaponBaseVData->m_flInaccuracyJumpInitial" );
	SCHEMA( float, jump_apex, "CCSWeaponBaseVData->m_flInaccuracyJumpApex" );

	SCHEMA( int, max_clip_1, "CBasePlayerWeaponVData->m_iMaxClip1" );
	SCHEMA( int, damage, "CCSWeaponBaseVData->m_nDamage" );
	SCHEMA( int, num_bullets, "CCSWeaponBaseVData->m_nNumBullets" );
	SCHEMA( float, headshot_multiplier, "CCSWeaponBaseVData->m_flHeadshotMultiplier" );
	SCHEMA( float, armor_ratio, "CCSWeaponBaseVData->m_flArmorRatio" );
	SCHEMA( float, penetration, "CCSWeaponBaseVData->m_flPenetration" );
	SCHEMA( float, range_modifier, "CCSWeaponBaseVData->m_flRangeModifier" );
	SCHEMA( int, zoom_fov_1, "CCSWeaponBaseVData->m_nZoomFOV1" );
	SCHEMA( int, zoom_fov_2, "CCSWeaponBaseVData->m_nZoomFOV2" );

	float get_inacc_move( int weapon_mode )
	{
		if ( weapon_mode < 0 || weapon_mode >= 2 )
			return inacc_fire( );

		return inacc_move( );
	}

	std::string name( )
	{
		OFFSET_FROM( weapon_name_offset, "CCSWeaponBaseVData->m_szName" );
		uintptr_t address_name = memory->read<uintptr_t>( ( uintptr_t ) this + weapon_name_offset );
		if ( !address_name )
			return "";
		return memory->read_string( ( uintptr_t ) address_name );
	}
};

class c_base_weapon : public c_base_entity
{
public:

	c_weapon_base_vdata* weapon_vdata( )
	{
		return ( c_weapon_base_vdata* )this->get_vdatabase( );
	}

	SCHEMA( bool, reloading, "C_CSWeaponBase->m_bInReload" );
	SCHEMA( int, zoom_level, "C_CSWeaponBaseGun->m_zoomLevel" );
	SCHEMA( int, clip_1, "C_BasePlayerWeapon->m_iClip1" );
	SCHEMA( int, clip_2, "C_BasePlayerWeapon->m_iClip2" );
	SCHEMA( float, acc_penalty, "C_CSWeaponBase->m_fAccuracyPenalty" );
	SCHEMA( float, turning_inacc, "C_CSWeaponBase->m_flTurningInaccuracy" );

	int item_definition_index( )
	{
		OFFSET_FROM( attribute_manager, "C_EconEntity->m_AttributeManager" );
		OFFSET_FROM( item, "C_AttributeContainer->m_Item" );
		OFFSET_FROM( item_def_index, "C_EconItemView->m_iItemDefinitionIndex" );

		return memory->read<int>( ( uintptr_t ) this + attribute_manager + item + item_def_index );
	}

	SCHEMA( int, weapon_mode, "C_CSWeaponBase->m_weaponMode" );
	SCHEMA( int, shots_fired_weapon, "C_CSWeaponBase->m_iShotsFired" );
	SCHEMA( float, recoil_index, "C_CSWeaponBase->m_flRecoilIndex" );

	float get_inaccuracy( c_player_pawn* pawn, c_weapon_base_vdata* vdata )
	{
		// todo: force spread? return the value
		// nospread? return 0

		static auto off_velocity = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_vecVelocity" ) ];
		static auto off_flags = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_fFlags" ) ];
		static auto off_move_type = schema_system->map_offsets[ FNV1A::HashConst( "C_BaseEntity->m_MoveType" ) ];
		static auto off_walking = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_bIsWalking" ) ];
		static auto off_max_speed = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flMaxSpeed" ) ];
		static auto off_inacc_crouch = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyCrouch" ) ];
		static auto off_inacc_stand = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyStand" ) ];
		static auto off_inacc_ladder = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyLadder" ) ];
		static auto off_inacc_move = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyMove" ) ];
		static auto off_jump_initial = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyJumpInitial" ) ];
		static auto off_jump_apex = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flInaccuracyJumpApex" ) ];
		static auto off_rec_crouch = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flRecoveryTimeCrouch" ) ];
		static auto off_rec_stand = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flRecoveryTimeStand" ) ];
		static auto off_rec_crouch_final = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flRecoveryTimeCrouchFinal" ) ];
		static auto off_rec_stand_final = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_flRecoveryTimeStandFinal" ) ];
		static auto off_rec_trans_start = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_nRecoveryTransitionStartBullet" ) ];
		static auto off_rec_trans_end = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_nRecoveryTransitionEndBullet" ) ];
		static auto off_is_revolver = schema_system->map_offsets[ FNV1A::HashConst( "CCSWeaponBaseVData->m_bIsRevolver" ) ];
		static auto off_accuracy_penalty = schema_system->map_offsets[ FNV1A::HashConst( "C_CSWeaponBase->m_fAccuracyPenalty" ) ];
		static auto off_turning_inaccuracy = schema_system->map_offsets[ FNV1A::HashConst( "C_CSWeaponBase->m_flTurningInaccuracy" ) ];
		static auto off_recoil_index = schema_system->map_offsets[ FNV1A::HashConst( "C_CSWeaponBase->m_iRecoilIndex" ) ];

		const int fire_mode = weapon_mode( );

		struct
		{
			vector3d velocity = {};
			uint32_t flags = 0;
			uint8_t  move_type = 0;
			bool is_walking = false;
			bool is_revolver = false;
			float max_spd = 0.f;
			float inacc_crouch = 0.f;
			float inacc_stand = 0.f;
			float inacc_ladder = 0.f;
			float inacc_move = 0.f;
			float inacc_jump_init = 0.f;
			float inacc_jump_apex = 0.f;
			float rec_crouch = 0.f;
			float rec_stand = 0.f;
			float rec_crouch_final = 0.f;
			float rec_stand_final = 0.f;
			int rec_trans_start = 0;
			int rec_trans_end = 0;
			float accuracy_penalty = 0.f;
			float turn_inaccuracy = 0.f;
			int recoil_index = 0;
		} d;

		const auto pawn_base = reinterpret_cast< uintptr_t >( pawn );
		const auto vdata_base = reinterpret_cast< uintptr_t >( vdata );
		const auto weap_base = reinterpret_cast< uintptr_t >( this );
		const size_t fm_off = fire_mode ? sizeof( float ) : 0;

		{
			c_read_batch batch;
			batch.queue<vector3d>( pawn_base + off_velocity, &d.velocity )
				.queue<uint32_t>( pawn_base + off_flags, &d.flags )
				.queue<uint8_t>( pawn_base + off_move_type, &d.move_type )
				.queue<bool>( pawn_base + off_walking, &d.is_walking )
				.queue<bool>( vdata_base + off_is_revolver, &d.is_revolver )
				.queue<float>( vdata_base + off_max_speed + fm_off, &d.max_spd )
				.queue<float>( vdata_base + off_inacc_crouch + fm_off, &d.inacc_crouch )
				.queue<float>( vdata_base + off_inacc_stand + fm_off, &d.inacc_stand )
				.queue<float>( vdata_base + off_inacc_ladder + fm_off, &d.inacc_ladder )
				.queue<float>( vdata_base + off_inacc_move + fm_off, &d.inacc_move )
				.queue<float>( vdata_base + off_jump_initial, &d.inacc_jump_init )
				.queue<float>( vdata_base + off_jump_apex, &d.inacc_jump_apex )
				.queue<float>( vdata_base + off_rec_crouch, &d.rec_crouch )
				.queue<float>( vdata_base + off_rec_stand, &d.rec_stand )
				.queue<float>( vdata_base + off_rec_crouch_final, &d.rec_crouch_final )
				.queue<float>( vdata_base + off_rec_stand_final, &d.rec_stand_final )
				.queue<int>( vdata_base + off_rec_trans_start, &d.rec_trans_start )
				.queue<int>( vdata_base + off_rec_trans_end, &d.rec_trans_end )
				.queue<float>( weap_base + off_accuracy_penalty, &d.accuracy_penalty )
				.queue<float>( weap_base + off_turning_inaccuracy, &d.turn_inaccuracy )
				.queue<int>( weap_base + off_recoil_index, &d.recoil_index );
			memory->flush_batch( batch );
		}

		const bool on_ground = ( d.flags & 1 ) != 0;
		const bool crouching = ( d.flags & 2 ) != 0;

		float base_inaccuracy;
		if ( d.move_type == 9 )
			base_inaccuracy = d.inacc_stand + d.inacc_ladder;
		else if ( crouching )
			base_inaccuracy = d.inacc_crouch;
		else
			base_inaccuracy = d.inacc_stand;

		float recovery {};
		if ( d.move_type == 9 )
		{
			recovery = d.rec_stand;
		}
		else if ( !on_ground )
		{
			recovery = d.rec_crouch * 4.f;
		}
		else
		{
			float base_rec, final_rec;
			if ( crouching )
			{
				base_rec = d.rec_crouch;
				final_rec = d.rec_crouch_final;
			}
			else
			{
				base_rec = d.rec_stand;
				final_rec = d.rec_stand_final;
			}

			if ( final_rec == -1.f )
			{
				recovery = base_rec;
			}
			else if ( d.recoil_index <= d.rec_trans_start )
			{
				recovery = base_rec;
			}
			else if ( d.recoil_index >= d.rec_trans_end )
			{
				recovery = final_rec;
			}
			else
			{
				recovery = base_rec + ( static_cast< float >( d.recoil_index - d.rec_trans_start ) /
					static_cast< float >( d.rec_trans_end - d.rec_trans_start ) ) * ( final_rec - base_rec );
			}
		}

		const float speed = std::sqrtf( d.velocity.x * d.velocity.x + d.velocity.y * d.velocity.y );
		const float edge0 = d.max_spd * 0.34f;
		const float edge1 = d.max_spd * 0.95f;

		float move_factor = 0.f;
		if ( edge0 == edge1 )
			move_factor = ( speed - edge1 >= 0.f ) ? 1.f : 0.f;
		else
			move_factor = std::clamp( ( speed - edge0 ) / ( edge1 - edge0 ), 0.f, 1.f );

		float move_inaccuracy = 0.f;
		if ( move_factor > 0.f )
		{
			if ( !d.is_walking )
				move_factor = std::powf( move_factor, 0.25f );

			move_inaccuracy = move_factor * d.inacc_move;
		}

		float total = d.turn_inaccuracy + move_inaccuracy + base_inaccuracy;

		if ( d.move_type != 9 && !on_ground )
		{
			const float jump_impulse = 301.993378; // sv_jump_impulse
			const float sqrt_threshold = std::sqrtf( std::fabsf( jump_impulse ) );
			const float sqrt_vertical = std::sqrtf( std::fabsf( d.velocity.z ) );
			const float lo = sqrt_threshold * 0.25f;

			float air;
			if ( lo == sqrt_threshold )
				air = ( sqrt_vertical - sqrt_threshold >= 0.f ) ? d.inacc_jump_init : d.inacc_jump_apex;
			else
			{
				const float frac = ( sqrt_vertical - lo ) / ( sqrt_threshold - lo );
				air = d.inacc_jump_apex + frac * ( d.inacc_jump_init - d.inacc_jump_apex );
			}

			if ( air < 0.f )
				air = 0.f;
			else
				air = std::fminf( d.inacc_jump_init * 2.f, air );

			total += air;
		}

		total += d.accuracy_penalty;

		return std::fminf( 1.f, total );
	}
};

class cs_weapon_info
{
public:
	float get_inaccuracy(c_base_weapon* p_weapon, float max_speed);
	float inaccuracy_strafe();
};