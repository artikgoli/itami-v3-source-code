#pragma once

// repaste v
inline std::filesystem::path get_cs2_game_dir( )
{
    HKEY hk;
    char buf[ MAX_PATH ] = {};
    DWORD sz = sizeof( buf );
    if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Valve\\Steam",
                        0, KEY_READ, &hk ) == ERROR_SUCCESS )
    {
        RegQueryValueExA( hk, "SteamPath", nullptr, nullptr, (LPBYTE)buf, &sz );
        RegCloseKey( hk );
    }
    if ( !buf[ 0 ] ) return {};

    auto try_lib = []( const std::filesystem::path& lib ) -> std::filesystem::path
    {
        auto p = lib / "steamapps" / "common" / "Counter-Strike Global Offensive";
        return std::filesystem::exists( p / "game" / "csgo" ) ? p : std::filesystem::path{};
    };

    auto r = try_lib( buf );
    if ( !r.empty( ) ) return r;

    std::ifstream lf( std::filesystem::path( buf ) / "config" / "libraryfolders.vdf" );
    std::string line;
    while ( std::getline( lf, line ) )
    {
        auto p = line.find( "\"path\"" );
        if ( p == std::string::npos ) continue;
        auto q1 = line.find( '"', p + 6 );
        if ( q1 == std::string::npos ) continue;
        auto q2 = line.find( '"', q1 + 1 );
        if ( q2 == std::string::npos ) continue;
        r = try_lib( line.substr( q1 + 1, q2 - q1 - 1 ) );
        if ( !r.empty( ) ) return r;
    }
    return {};
}

struct vpk_tree_reader
{
    std::vector<uint8_t> tree {};
    uint32_t             tree_size = 0;
    size_t               pos       = 0;

    bool open( const std::filesystem::path& vpk_dir )
    {
        std::ifstream df( vpk_dir / "pak01_dir.vpk", std::ios::binary );
        if ( !df ) return false;

        uint8_t hdr[ 28 ] = {};
        if ( !df.read( (char*)hdr, 28 ) ) return false;

        auto r32 = [&]( int o ) { return *reinterpret_cast<const uint32_t*>( hdr + o ); };
        if ( r32( 0 ) != 0x55AA1234u || r32( 4 ) != 2u ) return false;

        tree_size = r32( 8 );
        tree.resize( tree_size );
        return df.read( (char*)tree.data( ), tree_size ).good( );
    }

    std::string read_cstr( )
    {
        std::string s;
        while ( pos < tree_size && tree[ pos ] ) s += (char)tree[ pos++ ];
        if ( pos < tree_size ) ++pos;
        return s;
    }

    uint16_t r16( size_t o ) const { return *reinterpret_cast<const uint16_t*>( tree.data( ) + o ); }
    uint32_t r32( size_t o ) const { return *reinterpret_cast<const uint32_t*>( tree.data( ) + o ); }
};

inline std::vector<uint8_t> vpk_read_entry(
    const std::filesystem::path& vpk_dir,
    std::ifstream&               df,
    uint32_t                     tree_size,
    uint16_t                     arc_idx,
    uint32_t                     arc_off,
    uint32_t                     arc_len,
    const uint8_t*               preload_ptr,
    uint16_t                     preload_len )
{
    std::vector<uint8_t> result( preload_ptr, preload_ptr + preload_len );

    if ( arc_len == 0 ) return result;

    if ( arc_idx == 0x7FFF )
    {
        df.seekg( 28 + (std::streamoff)tree_size + arc_off );
        const size_t old = result.size( );
        result.resize( old + arc_len );
        if ( !df.read( (char*)result.data( ) + old, arc_len ) )
            result.resize( old );
    }
    else
    {
        char name[ 32 ];
        _snprintf_s( name, sizeof( name ), _TRUNCATE, "pak01_%03d.vpk", (int)arc_idx );
        std::ifstream arc( vpk_dir / name, std::ios::binary );
        if ( arc )
        {
            arc.seekg( arc_off );
            const size_t old = result.size( );
            result.resize( old + arc_len );
            if ( !arc.read( (char*)result.data( ) + old, arc_len ) )
                result.resize( old );
        }
    }
    return result;
}

inline std::vector<uint8_t> vpk_read_overhead_map(
    const std::filesystem::path& vpk_dir,
    const std::string&           map_name,
    std::string*                 out_found_name = nullptr )
{
    const std::string WANT_EXT  = "vtex_c";
    const std::string WANT_PATH = "panorama/images/overheadmaps";

    const std::array<std::string, 3> candidates = {
        map_name,
        map_name + "_radar",
        "overheadmap_" + map_name,
    };

    std::ifstream df( vpk_dir / "pak01_dir.vpk", std::ios::binary );
    if ( !df )
    {
        log_dbg( "vpk: cannot open pak01_dir.vpk in '%s'", vpk_dir.string( ).c_str( ) );
        return {};
    }

    uint8_t hdr[ 28 ] = {};
    if ( !df.read( (char*)hdr, 28 ) ) return {};

    auto r32h = [&]( int o ) { return *reinterpret_cast<const uint32_t*>( hdr + o ); };
    if ( r32h( 0 ) != 0x55AA1234u || r32h( 4 ) != 2u )
    {
        log_dbg( "vpk: bad signature/version in pak01_dir.vpk" );
        return {};
    }

    const uint32_t tree_size = r32h( 8 );
    std::vector<uint8_t> tree( tree_size );
    if ( !df.read( (char*)tree.data( ), tree_size ) ) return {};

    struct Entry
    {
        std::string name;
        uint16_t    arc_idx;
        uint32_t    arc_off, arc_len;
        uint32_t    preload_off; 
        uint16_t    preload_len;
    };
    std::vector<Entry> found_entries;

    size_t pos = 0;

    auto read_cstr = [&]( ) -> std::string
    {
        std::string s;
        while ( pos < tree_size && tree[ pos ] ) s += (char)tree[ pos++ ];
        if ( pos < tree_size ) ++pos;
        return s;
    };

    auto tr16 = [&]( size_t o ) { return *reinterpret_cast<const uint16_t*>( tree.data( ) + o ); };
    auto tr32 = [&]( size_t o ) { return *reinterpret_cast<const uint32_t*>( tree.data( ) + o ); };

    // Collect matching entries (single pass)
    [&]( )
    {
        while ( pos < tree_size )
        {
            const std::string cur_ext = read_cstr( );
            if ( cur_ext.empty( ) ) return;

            const bool ext_ok = ( cur_ext == WANT_EXT );

            while ( pos < tree_size )
            {
                const std::string cur_path = read_cstr( );
                if ( cur_path.empty( ) ) break;

                const bool path_ok = ext_ok && ( cur_path == WANT_PATH );

                while ( pos < tree_size )
                {
                    const std::string cur_file = read_cstr( );
                    if ( cur_file.empty( ) ) break;

                    if ( pos + 18 > tree_size ) return;

                    pos += 4; // crc32
                    const uint16_t preload = tr16( pos ); pos += 2;
                    const uint16_t aidx    = tr16( pos ); pos += 2;
                    const uint32_t aoff    = tr32( pos ); pos += 4;
                    const uint32_t alen    = tr32( pos ); pos += 4;
                    pos += 2; // terminator
                    const uint32_t poff    = (uint32_t)pos;
                    pos += preload;

                    if ( path_ok )
                        found_entries.push_back( { cur_file, aidx, aoff, alen, poff, preload } );
                }
            }
        }
    }( );

    log_dbg( "vpk: found %zu vtex_c files in overheadmaps (looking for '%s')",
             found_entries.size( ), map_name.c_str( ) );

    if ( found_entries.empty( ) )
        return {};

    const Entry* best = nullptr;

    for ( const auto& cand : candidates )
    {
        for ( const auto& e : found_entries )
        {
            if ( e.name == cand )
            {
                best = &e;
                break;
            }
        }
        if ( best ) break;
    }

    if ( !best )
    {
        for ( const auto& e : found_entries )
        {
            if ( e.name.find( map_name ) != std::string::npos )
            {
                best = &e;
                break;
            }
        }
    }

    if ( !best )
    {
        log_dbg( "vpk: no overheadmap match for '%s'", map_name.c_str( ) );
        return {};
    }

    log_dbg( "vpk: matched '%s.vtex_c' for map '%s'",
             best->name.c_str( ), map_name.c_str( ) );

    if ( out_found_name ) *out_found_name = best->name;

    return vpk_read_entry(
        vpk_dir, df, tree_size,
        best->arc_idx, best->arc_off, best->arc_len,
        tree.data( ) + best->preload_off, best->preload_len );
}

inline std::vector<uint8_t> vpk_read_file(
    const std::filesystem::path& vpk_dir,
    const std::string&           virt_path )
{
    auto dot_pos   = virt_path.rfind( '.' );
    auto slash_pos = virt_path.rfind( '/' );

    if ( dot_pos == std::string::npos ) return {};

    const std::string ext = virt_path.substr( dot_pos + 1 );
    std::string path_part, file_part;

    if ( slash_pos != std::string::npos && slash_pos < dot_pos )
    {
        path_part = virt_path.substr( 0, slash_pos );
        file_part = virt_path.substr( slash_pos + 1, dot_pos - slash_pos - 1 );
    }
    else
    {
        path_part = " ";
        file_part = virt_path.substr( 0, dot_pos );
    }

    std::ifstream df( vpk_dir / "pak01_dir.vpk", std::ios::binary );
    if ( !df ) return {};

    uint8_t hdr[ 28 ] = {};
    if ( !df.read( (char*)hdr, 28 ) ) return {};

    auto r32h = [&]( int o ) { return *reinterpret_cast<const uint32_t*>( hdr + o ); };
    if ( r32h( 0 ) != 0x55AA1234u || r32h( 4 ) != 2u ) return {};

    const uint32_t tree_size = r32h( 8 );
    std::vector<uint8_t> tree( tree_size );
    if ( !df.read( (char*)tree.data( ), tree_size ) ) return {};

    size_t pos = 0;

    auto read_cstr = [&]( ) -> std::string
    {
        std::string s;
        while ( pos < tree_size && tree[ pos ] ) s += (char)tree[ pos++ ];
        if ( pos < tree_size ) ++pos;
        return s;
    };

    auto tr16 = [&]( size_t o ) { return *reinterpret_cast<const uint16_t*>( tree.data( ) + o ); };
    auto tr32 = [&]( size_t o ) { return *reinterpret_cast<const uint32_t*>( tree.data( ) + o ); };

    std::vector<uint8_t> result;

    [&]( )
    {
        while ( pos < tree_size )
        {
            const std::string cur_ext = read_cstr( );
            if ( cur_ext.empty( ) ) return;

            while ( pos < tree_size )
            {
                const std::string cur_path = read_cstr( );
                if ( cur_path.empty( ) ) break;

                while ( pos < tree_size )
                {
                    const std::string cur_file = read_cstr( );
                    if ( cur_file.empty( ) ) break;

                    if ( pos + 18 > tree_size ) return;

                    pos += 4;
                    const uint16_t preload = tr16( pos ); pos += 2;
                    const uint16_t aidx    = tr16( pos ); pos += 2;
                    const uint32_t aoff    = tr32( pos ); pos += 4;
                    const uint32_t alen    = tr32( pos ); pos += 4;
                    pos += 2;
                    const uint32_t poff    = (uint32_t)pos;
                    pos += preload;

                    if ( cur_ext != ext || cur_path != path_part || cur_file != file_part )
                        continue;

                    result = vpk_read_entry(
                        vpk_dir, df, tree_size, aidx, aoff, alen,
                        tree.data( ) + poff, preload );
                    return; 
                }
            }
        }
    }( );

    return result;
}

inline int lz4_decompress_block(
    const uint8_t* src, int src_len,
    uint8_t*       dst, int dst_cap )
{
    const uint8_t* s    = src;
    const uint8_t* send = src + src_len;
    uint8_t*       d    = dst;
    uint8_t*       dlim = dst + dst_cap;

    while ( s < send )
    {
        const uint8_t token = *s++;

        int lit = token >> 4;
        if ( lit == 15 )
        {
            uint8_t x;
            do { if ( s >= send ) return -1; x = *s++; lit += x; } while ( x == 255 );
        }
        if ( d + lit > dlim || s + lit > send ) return -1;
        memcpy( d, s, lit );
        d += lit; s += lit;

        if ( s >= send ) 
            break; 

        if ( s + 2 > send ) return -1;
        const int off = (int)s[0] | ( (int)s[1] << 8 );
        s += 2;
        if ( off == 0 || d - off < dst ) return -1;

        int ml = ( token & 0xF ) + 4;
        if ( ( token & 0xF ) == 15 )
        {
            uint8_t x;
            do { if ( s >= send ) return -1; x = *s++; ml += x; } while ( x == 255 );
        }
        if ( d + ml > dlim ) return -1;
        const uint8_t* m = d - off;
        for ( int i = 0; i < ml; ++i ) *d++ = m[i]; 
    }
    return (int)( d - dst );
}


struct vtex_result
{
    std::vector<uint8_t> mip0_data {};
    int         width    = 0;
    int         height   = 0;
    DXGI_FORMAT dxgi_fmt = DXGI_FORMAT_UNKNOWN;
};

inline bool vtex_parse( const std::vector<uint8_t>& data, vtex_result& out )
{
    if ( data.size( ) < 16 ) return false;

    auto r16 = [&]( size_t o ) { return *reinterpret_cast<const uint16_t*>( data.data( ) + o ); };
    auto r32 = [&]( size_t o ) { return *reinterpret_cast<const uint32_t*>( data.data( ) + o ); };


    if ( r16( 4 ) != 12 ) 
        return false;

    const size_t   blocks_base = 8 + (size_t)r32( 8 );
    const uint32_t block_count = r32( 12 );

    size_t db_start = 0, db_size = 0;
    for ( uint32_t i = 0; i < block_count; ++i )
    {
        const size_t bep = blocks_base + (size_t)i * 12;
        if ( bep + 12 > data.size( ) ) break;
        if ( memcmp( data.data( ) + bep, "DATA", 4 ) == 0 )
        {
            db_start = ( bep + 4 ) + (size_t)r32( bep + 4 );
            db_size  = r32( bep + 8 );
            break;
        }
    }

    if ( !db_start || db_start + 40 > data.size( ) ) return false;

    const uint8_t* db = data.data( ) + db_start;

    const uint16_t tex_ver  = *reinterpret_cast<const uint16_t*>( db + 0 );
    if ( tex_ver != 1 )
    {
        log_dbg( "vtex: unexpected tex version %u", (unsigned)tex_ver );
        return false;
    }

    const uint16_t width    = *reinterpret_cast<const uint16_t*>( db + 20 );
    const uint16_t height   = *reinterpret_cast<const uint16_t*>( db + 22 );
    const uint8_t  vtfmt    = db[ 26 ];
    const uint8_t  num_mips = db[ 27 ];

    if ( !width || !height || !num_mips ) return false;

    DXGI_FORMAT fmt;
    switch ( vtfmt )
    {
    case  1: fmt = DXGI_FORMAT_BC1_UNORM;       break; // DXT1
    case  2: fmt = DXGI_FORMAT_BC3_UNORM;       break; // DXT5
    case  4: fmt = DXGI_FORMAT_R8G8B8A8_UNORM;  break; // RGBA8888
    case 19: fmt = DXGI_FORMAT_BC6H_UF16;       break; // BC6H
    case 20: fmt = DXGI_FORMAT_BC7_UNORM;       break; // BC7
    case 21: fmt = DXGI_FORMAT_BC5_UNORM;       break; // ATI2N (BC5)
    case 27: fmt = DXGI_FORMAT_BC4_UNORM;       break; // ATI1N (BC4)
    case 28: fmt = DXGI_FORMAT_B8G8R8A8_UNORM;  break; // BGRA8888
    default:
        log_dbg( "vtex: unsupported format %u (%dx%d)", (unsigned)vtfmt, (int)width, (int)height );
        return false;
    }

    auto mip_size = [&]( int mip ) -> size_t
    {
        const int mw = max( 1, (int)width  >> mip );
        const int mh = max( 1, (int)height >> mip );
        switch ( fmt )
        {
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC4_UNORM:
            return (size_t)( ( mw + 3 ) / 4 ) * ( ( mh + 3 ) / 4 ) * 8;
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC7_UNORM:
            return (size_t)( ( mw + 3 ) / 4 ) * ( ( mh + 3 ) / 4 ) * 16;
        default: // RGBA8888 / BGRA8888
            return (size_t)mw * mh * 4;
        }
    };

    const uint32_t extra_off = r32( db_start + 32 );
    const uint32_t num_extra = r32( db_start + 36 );

    std::vector<int32_t> comp_sizes;
    bool is_lz4 = false;

    if ( num_extra > 0 )
    {
        const size_t ebase = db_start + 32 + (size_t)extra_off;
        for ( uint32_t ei = 0; ei < num_extra && ei < 32; ++ei )
        {
            const size_t ep = ebase + (size_t)ei * 12;
            if ( ep + 12 > data.size( ) ) break;

            const uint32_t etype   = r32( ep );
            const uint32_t erawoff = r32( ep + 4 );
            const size_t   edata   = ( ep + 4 ) + (size_t)erawoff;

            if ( etype == 4u /* COMPRESSED_MIP_SIZE */ )
            {
                if ( edata + 12 > data.size( ) ) break;
                const uint32_t int1     = r32( edata );
                const uint32_t mips_off = r32( edata + 4 );
                const uint32_t mips_cnt = r32( edata + 8 );
                is_lz4 = ( int1 == 1 );
                const size_t sz_base = edata + 4 + (size_t)mips_off;
                if ( sz_base + (size_t)mips_cnt * 4 > data.size( ) ) break;
                comp_sizes.resize( mips_cnt );
                for ( uint32_t mi = 0; mi < mips_cnt; ++mi )
                    comp_sizes[ mi ] = (int32_t)r32( sz_base + (size_t)mi * 4 );
                break;
            }
        }
    }

    size_t file_pos = db_start + db_size;

    for ( int m = (int)num_mips - 1; m > 0; --m )
    {
        size_t skip = mip_size( m );
        if ( !comp_sizes.empty( ) && m < (int)comp_sizes.size( ) )
        {
            const size_t cs = (size_t)comp_sizes[ m ];
            if ( cs < skip ) skip = cs;
        }
        file_pos += skip;
    }

    const size_t mip0_uncomp = mip_size( 0 );
    const bool   mip0_lz4    = is_lz4
        && !comp_sizes.empty( )
        && (size_t)comp_sizes[ 0 ] < mip0_uncomp;

    if ( mip0_lz4 )
    {
        const size_t csz = (size_t)comp_sizes[ 0 ];
        if ( file_pos + csz > data.size( ) )
        {
            log_dbg( "vtex: lz4 mip0 oob (%zu+%zu > %zu)", file_pos, csz, data.size( ) );
            return false;
        }
        out.mip0_data.resize( mip0_uncomp );
        const int written = lz4_decompress_block(
            data.data( ) + file_pos, (int)csz,
            out.mip0_data.data( ),   (int)mip0_uncomp );
        if ( written != (int)mip0_uncomp )
        {
            log_dbg( "vtex: lz4 failed (want %zu got %d)", mip0_uncomp, written );
            return false;
        }
    }
    else
    {
        if ( file_pos + mip0_uncomp > data.size( ) )
        {
            log_dbg( "vtex: mip0 oob (%zu+%zu > %zu)", file_pos, mip0_uncomp, data.size( ) );
            return false;
        }
        out.mip0_data.assign(
            data.data( ) + file_pos,
            data.data( ) + file_pos + mip0_uncomp );
    }

    out.width    = (int)width;
    out.height   = (int)height;
    out.dxgi_fmt = fmt;
    log_dbg( "vtex: ok %dx%d fmt=%u mips=%u lz4=%d",
             (int)width, (int)height, (unsigned)vtfmt, (unsigned)num_mips, (int)mip0_lz4 );
    return true;
}

inline void vpk_parse_overview(
    const std::vector<uint8_t>& kv_bytes,
    float& pos_x, float& pos_y, float& scale )
{
    std::string s( kv_bytes.begin( ), kv_bytes.end( ) );

    auto kv_get = [&]( const char* key ) -> float
    {
        const auto p = s.find( key );
        if ( p == std::string::npos ) return 0.f;
        const auto q1 = s.find( '"', p + strlen( key ) );
        if ( q1 == std::string::npos ) return 0.f;
        const auto q2 = s.find( '"', q1 + 1 );
        if ( q2 == std::string::npos ) return 0.f;
        try { return std::stof( s.substr( q1 + 1, q2 - q1 - 1 ) ); }
        catch ( ... ) { return 0.f; }
    };

    pos_x = kv_get( "\"pos_x\"" );
    pos_y = kv_get( "\"pos_y\"" );
    scale = kv_get( "\"scale\"" );
}
