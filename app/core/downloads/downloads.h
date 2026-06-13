#pragma once

class c_download_manager
{
public:
    std::filesystem::path storage_path( ) const;
    std::filesystem::path maps_path( ) const;
    std::filesystem::path vphys_path( ) const;
    std::filesystem::path radar_path( ) const;

    bool ensure_storage( );

private:
    bool download_file( const std::wstring& url,
        const std::filesystem::path& out );

    bool extract_zip( const std::filesystem::path& zip,
        const std::filesystem::path& dest );
private:
    bool m_initialized = false;
};

inline auto download = std::make_unique<c_download_manager>( );
