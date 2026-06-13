void c_sdk::init( )
{
	m_client.resolve( );
	m_engine2.resolve( );
	m_tier0.resolve( );
	m_schema.resolve( );

	int count = (m_client.base != 0) + (m_engine2.base != 0) + (m_tier0.base != 0) + (m_schema.base != 0);

	constexpr int max_retries = 20; // 10 seconds
	int retries = 0;

	while ( count < 4 )
	{
		if ( retries >= max_retries )
		{
			std::string missing;
			if ( !m_client.base  ) missing += "  - client.dll\n";
			if ( !m_engine2.base ) missing += "  - engine2.dll\n";
			if ( !m_tier0.base   ) missing += "  - tier0.dll\n";
			if ( !m_schema.base  ) missing += "  - schemasystem.dll\n";

			std::string msg = "failed to find the following modules:\n\n" + missing;
			MessageBoxA( NULL, msg.c_str( ), "error", MB_OK | MB_ICONERROR );
			ExitProcess( EXIT_FAILURE );
			return;
		}

		memory->clear_modules( );
		m_client.resolve( );
		m_engine2.resolve( );
		m_tier0.resolve( );
		m_schema.resolve( );

		count = ( m_client.base != 0 ) + ( m_engine2.base != 0 ) + ( m_tier0.base != 0 ) + ( m_schema.base != 0 );
		Sleep( 500 );
		++retries;
	}

	log_dbg( "captured %d modules", count );

	//log_dbg( "client.dll base: 0x%p", ( void* ) m_client.base );
	//log_dbg( "engine2.dll base: 0x%p", ( void* ) m_engine2.base );
	//log_dbg( "tier0.dll base: 0x%p", ( void* ) m_tier0.base );
	//log_dbg( "schemasystem.dll base: 0x%p", ( void* ) m_schema.base );


	const int build_number = memory->read<int>(m_engine2.base + cs2_dumper::offsets::engine2_dll::dwBuildNumber);
	if (!build_number)
	{
		MessageBoxA(NULL, "failed to get build number", "error", MB_OK | MB_ICONERROR);
		ExitProcess(EXIT_FAILURE);
		return;
	}

	if (build_number != m_game_build)
	{
		std::stringstream ss;
		ss << "( " << m_game_build << " ) -> [ " << build_number << " ] " << "build number mismatch\n\n some features might not work or none will work at all \n\n proceed anyway?";
		std::string msg = ss.str();

		int result = MessageBoxA(NULL, msg.c_str(), "error", MB_YESNO | MB_ICONWARNING);
		if (result == IDYES)
		{
			// das y we keep it goin
		}
		else
		{
			ExitProcess(EXIT_FAILURE);
			return;
		}
	}

	log_dbg("build number: %d", build_number);

	schema_system = signature_t( memory->pattern_scan( "schemasystem.dll", X( "48 89 05 ? ? ? ? 4C 8D 0D ? ? ? ? 33 C0 48 C7 05 ? ? ? ? ? ? ? ? 89 05" ), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7 ) );
	convars = signature_t( memory->pattern_scan( "tier0.dll", X( "48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC E9" ), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7 ) );
	dw_global_vars = signature_t( memory->pattern_scan( "client.dll", X( "48 8B 05 ? ? ? ? 8B 48 ? FF C1 89 8F" ), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7 ) );
	local_controller = signature_t( memory->pattern_scan( "client.dll", X( "48 8D 0D ? ? ? ? 33 F6 4C 89 34 F9" ), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7 ) );
	view_matrix = signature_t( memory->pattern_scan( "client.dll", X( "48 8D 0D ? ? ? ? 48 89 44 24 ? 48 89 4C 24 ? 4C 8D 0D" ), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7 ) );

	//log_dbg("[SIG] schema system: 0x%p", (void*)sigs.schema_system);
	//log_dbg("[SIG] convars: 0x%p", (void*)sigs.convars);
	//log_dbg("[SIG] dw_global_vars: 0x%p", (void*)sigs.dw_global_vars);
	//log_dbg("[SIG] local_controller: 0x%p", (void*)sigs.local_controller);
	//log_dbg("[SIG] view_matrix: 0x%p", (void*)sigs.view_matrix);

	// todo dodac local pawn entity liste i viewangles
}
