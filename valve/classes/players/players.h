#pragma once
#include "c_base_weapon.h"

#define IN_ATTACK (1 << 0)
#define IN_JUMP (1 << 1)
#define IN_DUCK (1 << 2)
#define IN_FORWARD (1 << 3)
#define IN_BACK (1 << 4)
#define IN_USE (1 << 5)
#define IN_TURNLEFT (1 << 7)
#define IN_TURNRIGHT (1 << 8)
#define IN_MOVELEFT (1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2 (1 << 11)
#define IN_RELOAD (1 << 13)
#define IN_SPEED (1 << 16)
#define IN_SCORE (1 << 33)
#define IN_ZOOM (1 << 34)

class c_movement_services
{
public:
	// never changes
	SCHEMA( int, impulse, "CPlayer_MovementServices->m_nImpulse" );

	// accurate
	[[nodiscard]] __forceinline int buttons( ) noexcept
	{
		// for some reason this is 8bytes off?
		static std::uintptr_t uOffset = schema_system->map_offsets[ FNV1A::HashConst( "CPlayer_MovementServices->m_nButtons" ) ];
		return memory->read<int>( reinterpret_cast<std::uintptr_t>( this ) + uOffset + 0x8U );
	};

	// almost never changes
	SCHEMA( int, queued_buttons_mask, "CPlayer_MovementServices->m_nQueuedButtonDownMask" );

	// never changes
	SCHEMA( int, queued_buttons_change_mask, "CPlayer_MovementServices->m_nQueuedButtonChangeMask" );

	SCHEMA( float, max_speed, "CPlayer_MovementServices->m_flMaxspeed" );
};

class c_weapon_services
{
public:
	SCHEMA( c_base_handle, active_weapon_handle, "CPlayer_WeaponServices->m_hActiveWeapon" );
	SCHEMA( float, next_attack, "CCSPlayer_WeaponServices->m_flNextAttack" );

};

class c_player_item_services
{
public:
	SCHEMA( bool, has_defuser, "CCSPlayer_ItemServices->m_bHasDefuser" );
	SCHEMA( bool, has_helmet, "CCSPlayer_ItemServices->m_bHasHelmet" );
};

class c_camera_services
{
public:
	SCHEMA( int, fov, "CCSPlayerBase_CameraServices->m_iFOV" );
};

class c_player_pawn : public c_base_entity
{
public:
	SCHEMA(int, get_health, "C_BaseEntity->m_iHealth");
	SCHEMA(int, get_armor, "C_CSPlayerPawn->m_ArmorValue");
	SCHEMA(int, get_max_health, "C_BaseEntity->m_iMaxHealth");
	SCHEMA(int, team_num, "C_BaseEntity->m_iTeamNum");
	SCHEMA(int, life_state, "C_BaseEntity->m_lifeState");
	SCHEMA(int, shots_fired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA( vector3d, aim_punch_angle, "C_CSPlayerPawn->m_aimPunchAngle" );
	SCHEMA( c_base_weapon*, active_weapon, "C_CSPlayerPawn->m_pClippingWeapon" );
	SCHEMA( c_base_handle, ground_entity, "C_BaseEntity->m_hGroundEntity" );
	SCHEMA( vector3d, velocity, "C_BaseEntity->m_vecVelocity" );
	SCHEMA( vector3d, abs_velocity, "C_BaseEntity->m_vecAbsVelocity" );
	SCHEMA( vector3d, eye_angles, "C_BaseEntity->m_angEyeAngles" );
	SCHEMA( vector3d, view_offset, "C_BaseModelEntity->m_vecViewOffset" );
	SCHEMA( bool, is_walking, "C_CSPlayerPawn->m_bIsWalking" );
	SCHEMA( bool, is_scoped, "C_CSPlayerPawn->m_bIsScoped" );
	SCHEMA( bool, is_defusing, "C_CSPlayerPawn->m_bIsDefusing" );
	SCHEMA( float, flash_duration, "C_CSPlayerPawnBase->m_flFlashDuration" );
	SCHEMA( bool, buy_menu, "C_CSPlayerPawn->m_bIsBuyMenuOpen" );
	SCHEMA( bool, deathmatch_immune, "C_CSPlayerPawn->m_bGunGameImmunity" );
	SCHEMA( float, flash_bang_time, "C_CSPlayerPawnBase->m_flFlashBangTime" );

	bool is_alive( )
	{
		return this->life_state( ) == 256 && this->get_health( ) > 0;
	}

	vector3d get_eye_position()
	{
		return this->game_scene_node()->origin() + this->view_offset();
	}

	c_base_entity* as_ent( )
	{
		return ( c_base_entity* ) this;
	}

	SCHEMA( c_movement_services*, movement_services, "C_BasePlayerPawn->m_pMovementServices");
	SCHEMA( c_weapon_services*, weapon_services, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA( c_player_item_services*, item_services, "C_BasePlayerPawn->m_pItemServices" );
	SCHEMA( c_camera_services*, camera_services, "C_BasePlayerPawn->m_pCameraServices" );

	std::string weapon_name( ) // <-- trzeba to zmienic na nazwy broni z huda @judaszPL, bo np r8 jest pokazany jako deagle, mp5-sd jest mp7 jebana pasta z valve nie bedziemy tego zmieniac manualnie
	{
		static std::uint32_t offset = schema_system->map_offsets[ FNV1A::HashConst( "C_CSPlayerPawn->m_pClippingWeapon" ) ];

		std::uintptr_t weapon_address = 0;
		weapon_address = memory->read( reinterpret_cast< std::uintptr_t >( this ) + offset, { 0x10,0x20 } );
		if (weapon_address == 0)
			return {};

		std::string str_weapon_name = memory->read_string( weapon_address );

		std::size_t index = str_weapon_name.find(X ( "_" ) );
		if (index == std::string::npos || str_weapon_name.empty( ))
			str_weapon_name = {};
		else
			str_weapon_name = str_weapon_name.substr( index + 1, str_weapon_name.size( ) - index - 1 );

		std::transform( str_weapon_name.begin( ), str_weapon_name.end( ), str_weapon_name.begin( ), [ ] ( unsigned char c )
			{
				return std::toupper( c );
			}
		);

		return str_weapon_name;
	}
};

class c_damage_service
{
public:
	SCHEMA( int, damage_list, "CCSPlayerController_DamageServices->m_DamageList" );
};

class c_action_tracking_service {
public:
	SCHEMA(int, total_damage, "CCSPlayerController_ActionTrackingServices->m_flTotalRoundDamageDealt");

};

class c_player_observer_service
{
public:
	SCHEMA( c_base_handle, observer_target, "CPlayer_ObserverServices->m_hObserverTarget" );
};

template <typename T>
struct repeated_ptr_field
{
	struct rep
	{
		int allocated {}; // 0x28
		T* elements[ ( 2147483647 - 2 * sizeof( int ) ) / sizeof( void* ) ];
	};

	void* area;
	int current_size;
	int total_size;// 0x20
	rep* _rep;
};

struct subtick_move_step
{
	void* vftable {};
	char pad1[ 0x8 ];
	uint32_t enabled_fields {};
	char pad2[ 0x4 ];
	uint64_t buttons {};
	bool pressed {};
	char pad3[ 0x3 ];
	float when {};
	float analog_forward_delta {};
	float analog_side_delta {};
	float analog_pitch_delta {};
	float analog_yaw_delta {};
};

class c_player_controller
{
public:
	SCHEMA(c_base_handle, get_pawn_handle, "CBasePlayerController->m_hPawn");
	SCHEMA( int, tickbase, "CBasePlayerController->m_nTickBase" );
	SCHEMA( int, desired_fov, "CBasePlayerController->m_iDesiredFOV" );

	std::string get_name() 
	{
		static std::uint32_t offset = schema_system->map_offsets[FNV1A::HashConst("CCSPlayerController->m_sSanitizedPlayerName")];
		uintptr_t s_player_name = memory->read<uintptr_t>(reinterpret_cast<std::uintptr_t>(this) + offset);
		if (!s_player_name)
			return { };

		std::string player_name = memory->read_string(s_player_name);
		if(player_name.empty())
			return { };

		std::transform( player_name.begin( ), player_name.end( ), player_name.begin( ), [ ] ( unsigned char c )
			{
				return std::toupper( c );
			}
		);

		return player_name;
	}

	SCHEMA(int, get_ping, "CCSPlayerController->m_iPing");
	SCHEMA(long long, get_steam_id, "CBasePlayerController->m_steamID");
	SCHEMA( c_damage_service*, damage_service, "CCSPlayerController->m_pDamageServices" );
	SCHEMA( c_action_tracking_service*, action_tracking_service, "CCSPlayerController->m_pActionTrackingServices");

	vector3d cmd_angles( )
	{
		// tutorial how to find
		// ; #STR: "cl: %d: %s\n", "cl: %d ===========================\n"
		// validate user command, 4 funcs down from createmove
		// first sub call
		/*
		local_pawn = get_local_player(0);
		pawn = local_pawn;
		if ( local_pawn )
		{
		  v5 = get_current_command_number(local_pawn);
		*/
		// function call in get_current cmd num
		/*
		 v2 = user_cmd_base;
		 v3 = v1;
		 v11 = user_cmd_base;
		 v12 = v1;
		 result = *((_QWORD *)user_cmd_base + v1);
		*/
		static const auto get_usercmd_base_ptr = memory->pattern_scan( "client.dll", "4C 8B 35 ? ? ? ? 4C 63 F8", SCAN_RESOLVE_RIP, 3, 7 );
		static const auto usercmd_array = memory->read<std::uintptr_t>( get_usercmd_base_ptr );

		vector3d cmd_angles {};

		const auto usercmd_array_read = memory->read<std::uintptr_t>( usercmd_array );
		if ( usercmd_array_read )
		{
			const auto sequence = memory->read<int>( usercmd_array_read + 0x5910 );
			const auto cmd_ptr = usercmd_array_read + static_cast< std::size_t >( 152 ) * ( sequence % 150 );

			if ( sequence > 0 && memory->read<int>( cmd_ptr + 8 ) == sequence )
			{
				const auto base_pb = memory->read<std::uintptr_t>( cmd_ptr + 0x40 );
				if ( base_pb )
				{
					const auto angles_ptr = memory->read<std::uintptr_t>( base_pb + 0x40 );
					if ( angles_ptr )
					{
						cmd_angles.x = memory->read<float>( angles_ptr + 0x18 );
						cmd_angles.y = memory->read<float>( angles_ptr + 0x1C );
					}
				}
			}
		}

		return cmd_angles;
	}

	int sequence( )
	{
		// same as cmd_angles
		static const auto get_usercmd_base_ptr = memory->pattern_scan( "client.dll", "4C 8B 35 ? ? ? ? 4C 63 F8", SCAN_RESOLVE_RIP, 3, 7 );
		static const auto usercmd_array = memory->read<std::uintptr_t>( get_usercmd_base_ptr );

		vector3d cmd_angles {};

		const auto usercmd_array_read = memory->read<std::uintptr_t>( usercmd_array );
		if ( usercmd_array_read )
		{
			const auto sequence = memory->read<int>( usercmd_array_read + 0x5910 );
			return sequence;
		}

		return {};
	}

	SCHEMA( c_player_observer_service*, observer_pawn, "CCSPlayerController->m_hObserverPawn" );
};