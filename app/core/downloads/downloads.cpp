#include "downloads.h"
#include "../config/config.h"
#include <shldisp.h>

static std::filesystem::path get_storage_internal( )
{
	auto p = storage_base( ) / "maps";
	std::filesystem::create_directories( p );
	return p;
}

std::filesystem::path c_download_manager::storage_path( ) const
{
	return get_storage_internal( );
}

std::filesystem::path c_download_manager::maps_path( ) const
{
	return storage_path( ) / "maps";
}

std::filesystem::path c_download_manager::vphys_path( ) const
{
	return storage_path( ) / "vphys";
}

std::filesystem::path c_download_manager::radar_path( ) const
{
	auto p = storage_path( ) / "radar";
	std::filesystem::create_directories( p );
	return p;
}

bool c_download_manager::download_file( const std::wstring& url,
	const std::filesystem::path& out )
{
	return SUCCEEDED(
		URLDownloadToFileW(
			nullptr,
			url.c_str( ),
			out.wstring( ).c_str( ),
			0,
			nullptr
		)
	);
}

bool c_download_manager::extract_zip( const std::filesystem::path& zip, const std::filesystem::path& dest )
{
	std::error_code ec;
	std::filesystem::create_directories( dest, ec );

	HRESULT coinit = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

	bool ok = false;

	IShellDispatch* shell = nullptr;
	if ( SUCCEEDED( CoCreateInstance( CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
		IID_IShellDispatch, reinterpret_cast<void**>( &shell ) ) ) )
	{
		VARIANT vzip {};
		vzip.vt     = VT_BSTR;
		vzip.bstrVal = SysAllocString( zip.wstring( ).c_str( ) );

		VARIANT vdest {};
		vdest.vt     = VT_BSTR;
		vdest.bstrVal = SysAllocString( dest.wstring( ).c_str( ) );

		Folder* pzip = nullptr;
		shell->NameSpace( vzip, &pzip );

		if ( pzip )
		{
			Folder* pdest_folder = nullptr;
			shell->NameSpace( vdest, &pdest_folder );

			if ( pdest_folder )
			{
				FolderItems* pitems = nullptr;
				pzip->Items( &pitems );

				if ( pitems )
				{
					long count = 0;
					pitems->get_Count( &count );

					VARIANT vitems {};
					vitems.vt     = VT_DISPATCH;
					vitems.pdispVal = pitems;

					VARIANT vopts {};
					vopts.vt   = VT_I4;
					vopts.lVal = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

					pdest_folder->CopyHere( vitems, vopts );

					for ( int i = 0; i < 120; ++i )
					{
						long n = 0;
						Folder* check = nullptr;
						if ( SUCCEEDED( shell->NameSpace( vdest, &check ) ) && check )
						{
							FolderItems* ci = nullptr;
							check->Items( &ci );
							if ( ci ) { ci->get_Count( &n ); ci->Release( ); }
							check->Release( );
						}
						if ( n >= count ) { ok = true; break; }
						Sleep( 500 );
					}

					pitems->Release( );
				}

				pdest_folder->Release( );
			}

			pzip->Release( );
		}

		SysFreeString( vzip.bstrVal );
		SysFreeString( vdest.bstrVal );
		shell->Release( );
	}

	if ( SUCCEEDED( coinit ) )
		CoUninitialize( );

	return ok;
}

static int read_local_version( const std::filesystem::path& path )
{
	std::ifstream in( path );
	int v = 0;
	in >> v;
	return v;
}

static void write_local_version( const std::filesystem::path& path, int v )
{
	std::ofstream out( path, std::ios::trunc );
	out << v;
}

static std::string download_text( const std::wstring& url )
{
	wchar_t tmp[ MAX_PATH ] {};
	GetTempPathW( MAX_PATH, tmp );

	auto tmp_file = std::filesystem::path( tmp ) / "version.tmp";

	if (!SUCCEEDED( URLDownloadToFileW(
		nullptr,
		url.c_str( ),
		tmp_file.wstring( ).c_str( ),
		0,
		nullptr ) ))
		return {};

	std::ifstream in( tmp_file );
	std::string text;
	std::getline( in, text );
	in.close( );

	std::filesystem::remove( tmp_file );
	return text;
}

bool c_download_manager::ensure_storage( )
{
    if (m_initialized)
        return true;

    auto storage = storage_path( );
    auto version_file = storage / "version.txt";

    const std::string server_ver_str =
        download_text( L"https://itami.fun/dls/maps/version.txt" );

    if (server_ver_str.empty( ))
    {
        log_dbg( "failed to fetch server version" );
        return false;
    }

    const int server_version = std::stoi( server_ver_str );
    const int local_version =
        std::filesystem::exists( version_file )
        ? read_local_version( version_file )
        : 0;

    if (local_version == server_version)
    {
        m_initialized = true;
        return true;
    }

    log_dbg( "updating storage" );

    MessageBoxA( nullptr,
        "hello amazon package delivery from itami fun\n"
        "please wait, this might take a moment",
        "itami",
        MB_OK | MB_ICONINFORMATION );

    std::filesystem::remove_all( storage / "maps" );
    std::filesystem::remove_all( storage / "vphys" );

	auto maps_zip = storage / "maps.zip";
    auto maps_vphys_zip = storage / "maps_vphys.zip";
    auto surface_txt = storage / "surface_data.txt";
    auto surface_game_txt = storage / "surface_data_game.txt";

    std::error_code ec;

    auto maps_dir = storage / "maps";
    std::filesystem::create_directories( maps_dir );

	if (!download_file(
		L"https://itami.fun/dls/maps/maps.zip",
		maps_zip ))
		return false;

	if (!extract_zip( maps_zip, maps_dir ))
		return false;

	std::filesystem::remove( maps_zip, ec );

    if (!download_file(
        L"https://itami.fun/dls/maps/map_phys.zip",
        maps_vphys_zip ))
        return false;

    if (!extract_zip( maps_vphys_zip, storage ))
        return false;

    std::filesystem::remove( maps_vphys_zip, ec );

    if (!download_file(
        L"https://itami.fun/dls/maps/surface_data.txt",
        surface_txt ))
        return false;

    if (!download_file(
        L"https://itami.fun/dls/maps/surface_data_game.txt",
        surface_game_txt ))
        return false;

    write_local_version( version_file, server_version );

    m_initialized = true;
    log_dbg( "storage ready (version synced)" );

    return true;
}

