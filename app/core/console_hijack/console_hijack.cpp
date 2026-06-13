#include "console_hijack.h"

std::string c_console_hijack::get_steam_install_path( )
{
	HKEY h_key;
	char path_buffer[ MAX_PATH ] {};
	DWORD path_size = sizeof( path_buffer );

	if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &h_key ) == ERROR_SUCCESS ) 
	{
		if ( RegQueryValueExA( h_key, "SteamPath", NULL, NULL, ( LPBYTE ) path_buffer, &path_size ) == ERROR_SUCCESS ) 
		{
			RegCloseKey( h_key );
			return std::string( path_buffer );
		}
		RegCloseKey( h_key );
	}
	return "";
}

std::string c_console_hijack::extract_path_value( const std::string& line )
{
	size_t path_key_pos = line.find( "\"path\"" );
	if ( path_key_pos == std::string::npos )
		return "";

	size_t first_quote = line.find( '"', path_key_pos + 6 );
	if ( first_quote == std::string::npos )
		return "";

	size_t second_quote = line.find( '"', first_quote + 1 );
	if ( second_quote == std::string::npos ) 
		return "";

	return line.substr( first_quote + 1, second_quote - first_quote - 1 );
}

std::vector<std::filesystem::path> c_console_hijack::get_steam_libraries( const std::string& steam_path )
{
	std::vector<std::filesystem::path> libraries;
	std::filesystem::path config_path = std::filesystem::path( steam_path ) / "config" / "libraryfolders.vdf";

	libraries.push_back( std::filesystem::path( steam_path ) );

	std::ifstream file( config_path );
	if ( !file.is_open( ) ) 
		return libraries;

	std::string line;
	while ( std::getline( file, line ) ) 
	{
		std::string raw_path = extract_path_value( line );
		if ( !raw_path.empty( ) ) 
		{
			std::filesystem::path clean_path( raw_path );
			if ( std::filesystem::exists( clean_path ) && clean_path != libraries[ 0 ] )
				libraries.push_back( clean_path );
		}
	}
	return libraries;
}

std::filesystem::path c_console_hijack::find_csgo_installation( const std::vector<std::filesystem::path>& libraries )
{
	for ( const auto& lib : libraries ) 
	{
		std::filesystem::path csgo_path = lib / "steamapps" / "common" / "Counter-Strike Global Offensive";
		if ( std::filesystem::exists( csgo_path ) )
			return csgo_path;
	}
	return "";
}

bool c_console_hijack::is_line_in_file( const std::filesystem::path& file_path, const std::string& search_string )
{
	std::ifstream file( file_path );
	if ( !file.is_open( ) ) 
		return false;

	std::string line;
	while ( std::getline( file, line ) ) 
		if ( line.find( search_string ) != std::string::npos )
			return true;

	return false;
}

void c_console_hijack::generate_custom_binds( const std::filesystem::path& path )
{
	std::ofstream new_file( path / "method.cfg", std::ios::out | std::ios::trunc );
	if ( new_file.is_open( ) )
	{
		new_file << "alias itmi_bind_jmp \"bind scancode44 +jump\"" << "\n";
		new_file << "alias itmi_unbind_jmp \"unbind scancode44\"" << "\n";

		binds[ e_hijacked_bind::FORCE_JUMP ] = VK_F13;
		binds[ e_hijacked_bind::REMOVE_JUMP ] = VK_F14;

		new_file << "bind F13 itmi_bind_jmp" << "\n";
		new_file << "bind F14 itmi_unbind_jmp" << "\n";

		new_file.close( );
	}
	log_dbg( "1337 haxor method generated" );
}

void c_console_hijack::init( )
{
	std::string steam_base = get_steam_install_path( );
	std::vector<std::filesystem::path> libraries = get_steam_libraries( steam_base );
	std::filesystem::path csgo_dir = find_csgo_installation( libraries );
	if ( csgo_dir.empty( ) )
		log_dbg( "wtf csgo not found???" );

	log_dbg( "captured game folder" );

	std::filesystem::path cfg_folder = csgo_dir / "game" / "csgo" / "cfg";
	if ( !std::filesystem::exists( cfg_folder ) )
	{
		MessageBoxA( 0, "your game install is fucked", "wow", MB_OK );
		ExitProcess( 0 );
		return;
	}

	log_dbg( "config folder: %s", cfg_folder.string( ).data( ) );
	generate_custom_binds( cfg_folder );

	auto hijack_cfg = cfg_folder / "gamemode_competitive.cfg";
	if ( !std::filesystem::exists( hijack_cfg ) )
	{
		MessageBoxA( 0, "your game install is fucked case 2", "wow", MB_OK );
		ExitProcess( 0 );
		return;
	}

	log_dbg( "mainframe hacked!" );

	if ( !is_line_in_file( hijack_cfg, "exec method.cfg" ) )
	{
		std::ofstream file( hijack_cfg, std::ios::app );
		if ( file.is_open( ) )
		{
			file << "\n" << "exec method.cfg" << "\n";
			file.close( );
		}
	}

	log_dbg( "1080p 60fps mainframe hack sucess (im in)" );

}