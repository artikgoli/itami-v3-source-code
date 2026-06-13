#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "bcrypt.lib")
#ifdef _PRODUCTION
#pragma comment(lib, "VMProtectSDK64.lib")
#endif

#include <wininet.h>
#include <bcrypt.h>
#include "utilities/vmprotect/VMProtectSDK.h"

void c_security::start_heartbeat( )
{
	if (running)
		return;

	running = true;
	heartbeat_thread = std::thread( &c_security::heartbeat_internal, this );
}

void c_security::stop_heartbeat( )
{
	running = false;

	if (heartbeat_thread.joinable( ))
		heartbeat_thread.join( );
}

std::string build_json_body( )
{
	VMProtectBeginVirtualization( "build_json_body" );
	const int64_t ts =
		std::chrono::duration_cast< std::chrono::seconds >(
			std::chrono::system_clock::now( ).time_since_epoch( )
		).count( );

	std::string result = std::string( "{\"username\":\"" ) +
		g_security->m_user.m_username +
		"\",\"timestamp\":" +
		std::to_string( ts ) +
		"}";
	VMProtectEnd( );
	return result;
}

static std::string aes_128_ecb( const std::string& input, bool encrypt )
{
	VMProtectBeginVirtualization( "aes_128_ecb" );
	if ( input.empty( ) )
		return "";

	BCRYPT_ALG_HANDLE h_alg = nullptr;
	BCRYPT_KEY_HANDLE h_key = nullptr;
	std::string result;

	do
	{
		if ( !BCRYPT_SUCCESS( BCryptOpenAlgorithmProvider( &h_alg, BCRYPT_AES_ALGORITHM, nullptr, 0 ) ) )
			break;

		if ( !BCRYPT_SUCCESS( BCryptSetProperty( h_alg, BCRYPT_CHAINING_MODE,
			( PUCHAR ) BCRYPT_CHAIN_MODE_ECB, sizeof( BCRYPT_CHAIN_MODE_ECB ), 0 ) ) )
			break;

		ULONG obj_len = 0, cb = 0;
		if ( !BCRYPT_SUCCESS( BCryptGetProperty( h_alg, BCRYPT_OBJECT_LENGTH,
			( PUCHAR ) &obj_len, sizeof( ULONG ), &cb, 0 ) ) )
			break;

		std::vector<uint8_t> key_obj( obj_len );
		const char key[ ] = "0a0se4s9eqkmcned";

		if ( !BCRYPT_SUCCESS( BCryptGenerateSymmetricKey( h_alg, &h_key,
			key_obj.data( ), obj_len, ( PUCHAR ) key, 16, 0 ) ) )
			break;

		auto fn = encrypt ? BCryptEncrypt : BCryptDecrypt;

		ULONG out_len = 0;
		fn( h_key, ( PUCHAR ) input.data( ), ( ULONG ) input.size( ),
			nullptr, nullptr, 0, nullptr, 0, &out_len, BCRYPT_BLOCK_PADDING );

		std::vector<uint8_t> output( out_len );
		if ( BCRYPT_SUCCESS( fn( h_key, ( PUCHAR ) input.data( ), ( ULONG ) input.size( ),
			nullptr, nullptr, 0, output.data( ), out_len, &out_len, BCRYPT_BLOCK_PADDING ) ) )
			result = std::string( ( char* ) output.data( ), out_len );

	} while ( false );

	if ( h_key ) BCryptDestroyKey( h_key );
	if ( h_alg ) BCryptCloseAlgorithmProvider( h_alg, 0 );

	VMProtectEnd( );
	return result;
}

void c_security::heartbeat_internal( )
{
	VMProtectBegin( "heartbeat_internal" );
	auto now_seconds = [ ] ( )
		{
			return std::chrono::duration_cast< std::chrono::seconds >(
				std::chrono::steady_clock::now( ).time_since_epoch( )
			).count( );
		};

	int64_t last_success = now_seconds( );

	HINTERNET h_internet = InternetOpenA( "XTCLOVE", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0 );
	if ( !h_internet )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
		exit( 0 );
	}

	DWORD timeout = 5000;
	InternetSetOptionA( h_internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof( timeout ) );
	InternetSetOptionA( h_internet, INTERNET_OPTION_SEND_TIMEOUT,    &timeout, sizeof( timeout ) );
	InternetSetOptionA( h_internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof( timeout ) );

	while ( running )
	{
		bool success = false;

		const std::string encrypted = aes_128_ecb( build_json_body( ), true );

		HINTERNET h_connect = InternetConnectA( h_internet, "itami.fun",
			INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0 );

		if ( h_connect )
		{
			HINTERNET h_request = HttpOpenRequestA( h_connect, "POST", "/api/user/ADBBUTINACA",
				"HTTP/1.1", nullptr, nullptr, INTERNET_FLAG_SECURE, 0 );

			if ( h_request )
			{
				const char* hdr = "Content-Type: application/octet-stream\r\n";

				if ( HttpSendRequestA( h_request, hdr, ( DWORD ) strlen( hdr ),
					( LPVOID ) encrypted.data( ), ( DWORD ) encrypted.size( ) ) )
				{
					char code[ 16 ] = { 0 };
					DWORD code_sz = sizeof( code );
					HttpQueryInfoA( h_request, HTTP_QUERY_STATUS_CODE, code, &code_sz, nullptr );

					if ( strcmp( code, "200" ) == 0 )
					{
						char buf[ 4096 ];
						DWORD read = 0;
						std::string resp;

						while ( InternetReadFile( h_request, buf, sizeof( buf ), &read ) && read )
							resp.append( buf, read );

						if ( aes_128_ecb( resp, false ) == "\"HEARTBEAT_V_WITHIN_TIMEFRAME\"" )
							success = true;
					}
				}

				InternetCloseHandle( h_request );
			}

			InternetCloseHandle( h_connect );
		}

		if ( success )
		{
			last_success = now_seconds( );
		}
		else if ( now_seconds( ) - last_success > 15 )
		{
			InternetCloseHandle( h_internet );
			std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
			exit( 0 );
		}

		std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
	}

	InternetCloseHandle( h_internet );
	VMProtectEnd( );
}