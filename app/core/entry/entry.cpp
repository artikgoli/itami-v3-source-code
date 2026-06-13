#include "..\..\features\menu\menu.h"
#include "..\..\features\entity_cache\entity_cache.h"
#include "..\..\features\esp\esp.h"
#include "..\..\features\smoke\smoke.h"
#include "../downloads/downloads.h"
#include "../console_hijack/console_hijack.h"
#include "..\..\features\misc\misc.h"
#include "../../utilities/handler.h"
#include "../../utilities/memory/driver/driver.h"

static void game_loop( )
{
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 3 ) );

		if ( !FindWindowA( 0, "Counter-Strike 2" ) )
		{
			ExitProcess( EXIT_FAILURE );
			break;
		}
	}
}

static void entity_update_stub( )
{
	// we do this to keep TLS intact
	// this is a system similar to how unity does FixedUpdate
	// https://docs.unity3d.com/6000.0/Documentation/ScriptReference/MonoBehaviour.FixedUpdate.html
	// this also compensates for any frames lost so we dont lag behind

	using clock = std::chrono::high_resolution_clock;
	constexpr double tick_rate = 1.0 / 128.0; // l0l, 0.0066 vs 0.015625, we dont need to run it that fast, twice per tick
	constexpr auto delta_duration = std::chrono::duration<double>( tick_rate );
	constexpr auto max_accumulator = delta_duration * 8;

	entity_cache->tickrate = ( float ) tick_rate;

	double global_time = 0.0;
	auto accumulator = std::chrono::duration<double>( 0 );
	auto last_time = clock::now( );

	while ( true )
	{
		auto current_time = clock::now( );
		auto frame_time = current_time - last_time;
		last_time = current_time;

		accumulator += frame_time;

		if ( accumulator > max_accumulator )
			accumulator = max_accumulator;

		while ( accumulator >= delta_duration )
		{
			entity_cache->update( ( float ) global_time );
			smoke_system->update_bases( ( float ) global_time );

			global_time += tick_rate;
			accumulator -= delta_duration;
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}
}

void entry::init()
{
	SetUnhandledExceptionFilter( handler::on_exception );
	console_hijack->init( );

#ifndef MEMORY_USERMODE
	driver::setup();
	Sleep( 2000 );
#endif

	if ( !FindWindowA( 0, "Counter-Strike 2" ) )
	{
		MessageBoxA( nullptr, "waiting for cs2 bro )", "itami", MB_OK | MB_ICONWARNING );

		while ( !FindWindowA( 0, "Counter-Strike 2" ) )
			std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
	}

	Sleep( 3000 );
	memory->init( "cs2.exe" );
	download->ensure_storage( );
	sdk->init();
	schema_system->init();
	convar->init( );

	std::thread( &c_parser::init, parser ).detach( );

	menu = std::make_unique<c_menu>( );
	std::thread( game_loop ).detach( );
	std::thread( entity_update_stub ).detach( );
	esp->init( );
	smoke_system->init( );

	log_dbg("nice paste from github bro");

#ifndef _DEVELOPER
	g_security->start_heartbeat( );
#endif

}
