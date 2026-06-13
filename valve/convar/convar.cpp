#include "convar.h"

uintptr_t c_convar_manager::find(const std::string& name)
{
	FNV1A_t hash = FNV1A::Hash(name.c_str());

	auto it = cvar_map.find(hash);

	if (it != cvar_map.end())
		return it->second;

	return 0;
}

void c_convar_manager::set_monitored_cvars( )
{
	monitored_cvars.emplace_back( "sensitivity" );
	monitored_cvars.emplace_back( "mp_teammates_are_enemies" );
	monitored_cvars.emplace_back( "weapon_accuracy_forcespread" );
	monitored_cvars.emplace_back( "weapon_air_spread_scale" );
	monitored_cvars.emplace_back( "sv_jump_impulse" );
	monitored_cvars.emplace_back( "sv_strafing_inaccuracy_bias" );
	monitored_cvars.emplace_back( "sv_strafing_inaccuracy_scale" );
	monitored_cvars.emplace_back( "mp_damage_scale_ct_head" );
	monitored_cvars.emplace_back( "mp_damage_scale_t_head" );
	monitored_cvars.emplace_back( "mp_damage_scale_ct_body" );
	monitored_cvars.emplace_back( "mp_damage_scale_t_body" );
	monitored_cvars.emplace_back( "mp_damage_headshot_only" );
}

void c_convar_manager::init()
{
	set_monitored_cvars( );

	auto cvar_list_addr = sdk->convars.address;

	auto cvar_list = memory->read<c_utl_linked_list<uintptr_t>>(cvar_list_addr + 0x40);

	for (auto i = cvar_list.head(); i != c_utl_linked_list<uintptr_t>::invalid_index(); i = cvar_list.next(i))
	{
		uintptr_t cvar_address = cvar_list.element(i);
		if (!cvar_address)
			continue;

		cvar_t convar = memory->read<cvar_t>(cvar_address);

		std::string cvar_name = convar.get_name();
		if (cvar_name.empty())
			continue;

		for ( auto& c : monitored_cvars )
			if ( strcmp( cvar_name.data( ), c.data( ) ) == 0 )
				cvar_map[ FNV1A::Hash( cvar_name.c_str( ) ) ] = cvar_address;
	}

	if (cvar_map.empty())
	{
		MessageBoxA(NULL, "failed to capture convars - wait for cheat update", "error", MB_OK | MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
		return;
	}

	log_dbg("captured %s convars", std::to_string(cvar_map.size()).c_str());
}