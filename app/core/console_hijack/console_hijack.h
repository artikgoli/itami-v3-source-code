#pragma once

enum e_hijacked_bind
{
	FORCE_JUMP = 0x0,
	REMOVE_JUMP = 0x1
};

class c_console_hijack
{
public:
	void init( );

	std::unordered_map<e_hijacked_bind, int> binds {};

private:
	std::string get_steam_install_path( );
	std::string extract_path_value( const std::string& line );
	std::vector<std::filesystem::path> get_steam_libraries( const std::string& steam_path );
	std::filesystem::path find_csgo_installation( const std::vector<std::filesystem::path>& libraries );
	bool is_line_in_file( const std::filesystem::path& file_path, const std::string& search_string );

	void generate_custom_binds( const std::filesystem::path& path );
};

inline auto console_hijack = std::make_unique<c_console_hijack>( );