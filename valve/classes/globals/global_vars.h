#pragma once
class c_global_vars { // todo: clean this up ?
public:
    std::string level_name();
    float real_time( );
    float get_time( )
    {
        using clock = std::chrono::steady_clock;
        static auto start = clock::now( );
        return std::chrono::duration<float>( clock::now( ) - start ).count( );
    }
    bool team_cvar( )
    {
        return convar->get<bool>( "mp_teammates_are_enemies" );
    }
    std::wstring utf8_to_wide( const std::string& str )
    {
        if (str.empty( ))
            return {};

        int size_needed = MultiByteToWideChar( CP_UTF8, 0, str.data( ), ( int ) str.size( ), nullptr, 0 );

        std::wstring result( size_needed, 0 );

        MultiByteToWideChar( CP_UTF8, 0, str.data( ), ( int ) str.size( ), result.data( ), size_needed );

        return result;
    }
};
inline auto global_vars = std::make_shared<c_global_vars>();