#include "autowall.h"
#include "..\entity_cache\entity_cache.h"
#include "../../core/downloads/downloads.h"
#include "../notifications/notifications.h"

#undef max

void c_autowall::get_weapon_data( c_base_weapon* weapon, float& base_damage, float& range_modifier, float& pen_power, float& armor_modifier, float& headshot_mult )
{
	auto vdata = weapon->weapon_vdata( );
	if ( !vdata )
		return;

	base_damage = vdata->damage( );
	range_modifier = vdata->range_modifier( );
	pen_power = vdata->penetration( );
	armor_modifier = vdata->armor_ratio( );
	headshot_mult = vdata->headshot_multiplier( );
}

// todo: dodac hitboxy, typu
// tak jak w triggerbocie sprawdzic jaki hitbox interesct jest
// i multipliery zrobic
// + jak jest visible to wyjebac
// base_damage * hitbox_damage_multiplier

float c_autowall::handle_bullet_penetration( const vector3d& from, const vector3d& to,
	const float& base_damage, const float& range_modifier, const float& penetration_power, const int& hitbox )
{
	if ( cached_materials.empty( ) )
		init_materials( );

	float current_damage = base_damage;

	auto hit_data = parser->get_penetration_data( to, from );

	if ( hit_data.empty( ) )
	{
		float total_distance = from.dist( to );
		current_damage *= std::pow( range_modifier, total_distance / 500.0f );
		return std::max( current_damage, 0.0f );
	}

	vector3d current_pos = from;
	vector3d direction = ( to - from ).normalize_vector( );

	float total_thickness_cm = 0.0f;
	int walls_hit = 0;

	for ( const auto& hit : hit_data )
	{
		walls_hit++;
		float abs_entry_dist = std::get<0>( hit );
		int entry_mat_idx = std::get<1>( hit );
		float thickness = std::get<2>( hit );
		int exit_mat_idx = std::get<3>( hit );

		float step_distance = abs_entry_dist - from.dist( current_pos );
		current_damage *= std::pow( range_modifier, step_distance / 500.0f );

		auto entry_it = cached_materials.find( entry_mat_idx );
		auto exit_it  = cached_materials.find( exit_mat_idx );
		auto& entry = entry_it != cached_materials.end( ) ? entry_it->second : cached_materials[ 15 ];
		auto& exit_ = exit_it  != cached_materials.end( ) ? exit_it->second  : cached_materials[ 15 ];

		float entry_pen_mod = entry.penetration_damage_mod;
		float exit_pen_mod = exit_.penetration_damage_mod;
		float combined_pen_mod = 0.0f;

		if ( entry_mat_idx == exit_mat_idx && entry.same_mat_pen_override > 0.f )
			combined_pen_mod = entry.same_mat_pen_override;
		else
			combined_pen_mod = ( entry_pen_mod + exit_pen_mod ) / 2.0f;

		if ( combined_pen_mod == 0.0f )
			return 0.0f;

		float thickness_cm = thickness * 2.54f;
		float modifier = std::max( 0.0f, 1.0f / combined_pen_mod );
		float wall_penalty = 1.0f + ( walls_hit * 0.15f );

		float lost_damage = std::max(
			( ( modifier * thickness_cm * thickness_cm ) / 24.0f ) +
			( ( current_damage * 0.18f ) + std::max( 3.75f / penetration_power, 0.0f ) * 3.0f * modifier ),
			0.0f
		) * wall_penalty;

		current_damage -= lost_damage;

		if ( current_damage < 1.0f )
			return 0.0f;

		current_pos = from + ( direction * ( abs_entry_dist + thickness ) );
	}

	float remaining_distance = current_pos.dist( to );
	current_damage *= std::pow( range_modifier, remaining_distance / 500.0f );

	return std::max( current_damage, 0.0f );
}

std::vector<long long> c_autowall::extract_vphys_hashes( const std::string& path )
{
	std::ifstream file( path );
	std::string content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>( ) );

	std::vector<long long> hashes;

	auto start = content.find( "m_surfacePropertyHashes" );
	if ( start == std::string::npos )
		return hashes;

	start = content.find( '[', start );
	if ( start == std::string::npos )
		return hashes;

	auto end = content.find( ']', start );
	if ( end == std::string::npos )
		return hashes;

	const char* p = content.c_str( ) + start + 1;
	const char* e = content.c_str( ) + end;

	while ( p < e )
	{
		while ( p < e && !std::isdigit( ( unsigned char ) *p ) ) ++p;
		if ( p >= e ) break;
		char* next = nullptr;
		hashes.push_back( std::strtoll( p, &next, 10 ) );
		p = next;
	}

	return hashes;
}

std::unordered_map<long long, std::string> c_autowall::load_hash_to_name_map( const std::string& path )
{
	std::unordered_map<long long, std::string> map;
	map.reserve( 256 );

	std::ifstream file( path, std::ios::binary | std::ios::ate );
	if ( !file )
		return map;

	std::string content( ( size_t ) file.tellg( ), '\0' );
	file.seekg( 0 );
	file.read( content.data( ), ( std::streamsize ) content.size( ) );

	std::string current_name;

	auto find_in = []( const char* begin, const char* end, const char* kw, size_t kwlen ) -> const char*
	{
		if ( ( size_t ) ( end - begin ) < kwlen )
			return nullptr;
		for ( const char* p = begin; p + kwlen <= end; ++p )
			if ( *p == *kw && memcmp( p, kw, kwlen ) == 0 )
				return p;
		return nullptr;
	};

	const char* base = content.c_str( );
	const char* cend = base + content.size( );

	for ( const char* p = base; p < cend; )
	{
		const char* eol = ( const char* ) memchr( p, '\n', cend - p );
		if ( !eol ) eol = cend;

		const char* kp = find_in( p, eol, "surfacePropertyName", 19 );
		if ( kp )
		{
			const char* f = ( const char* ) memchr( kp, '"', eol - kp );
			if ( f )
			{
				++f;
				const char* l = eol - 1;
				while ( l > f && *l != '"' ) --l;
				if ( *l == '"' && l > f )
					current_name.assign( f, l - f );
			}
		}
		else
		{
			kp = find_in( p, eol, "m_nameHash", 10 );
			if ( kp )
			{
				const char* q = kp + 10;
				while ( q < eol && !std::isdigit( ( unsigned char ) *q ) ) ++q;
				if ( q < eol )
					map.emplace( std::strtoll( q, nullptr, 10 ), current_name );
			}
		}

		p = eol < cend ? eol + 1 : cend;
	}
	return map;
}

std::unordered_map<std::string, surface_phys_t> c_autowall::load_name_to_phys_map( const std::string& path )
{
	std::unordered_map<std::string, surface_phys_t> map;
	map.reserve( 256 );

	std::ifstream file( path, std::ios::binary | std::ios::ate );
	if ( !file )
		return map;

	std::string content( ( size_t ) file.tellg( ), '\0' );
	file.seekg( 0 );
	file.read( content.data( ), ( std::streamsize ) content.size( ) );

	surface_phys_t current;

	auto find_in = []( const char* begin, const char* end, const char* kw, size_t kwlen ) -> const char*
	{
		if ( ( size_t ) ( end - begin ) < kwlen )
			return nullptr;
		for ( const char* p = begin; p + kwlen <= end; ++p )
			if ( *p == *kw && memcmp( p, kw, kwlen ) == 0 )
				return p;
		return nullptr;
	};

	const char* base = content.c_str( );
	const char* cend = base + content.size( );

	for ( const char* p = base; p < cend; )
	{
		const char* eol = ( const char* ) memchr( p, '\n', cend - p );
		if ( !eol ) eol = cend;

		const char* kp = find_in( p, eol, "surfacePropertyName", 19 );
		if ( kp )
		{
			if ( !current.name.empty( ) )
				map.emplace( current.name, current );

			const char* f = ( const char* ) memchr( kp, '"', eol - kp );
			if ( f )
			{
				++f;
				const char* l = eol - 1;
				while ( l > f && *l != '"' ) --l;
				if ( *l == '"' && l > f )
					current.name.assign( f, l - f );
			}

			current.penetration_distance_mod = 1.0f;
			current.penetration_damage_mod   = 1.0f;

			if ( current.name == "cardboard" || current.name == "Wood" || current.name == "Wood_Plank" )
				current.same_mat_pen_override = 3.0f;
			else if ( current.name == "plastic" || current.name == "plastic_barrel" || current.name == "plastic_solid" )
				current.same_mat_pen_override = 2.0f;
			else
				current.same_mat_pen_override = 0.f;
		}
		else if ( ( kp = find_in( p, eol, "bulletPenetrationDistanceModifier", 33 ) ) )
		{
			const char* eq = ( const char* ) memchr( kp, '=', eol - kp );
			if ( eq )
				current.penetration_distance_mod = std::strtof( eq + 1, nullptr );
		}
		else if ( ( kp = find_in( p, eol, "bulletPenetrationDamageModifier", 31 ) ) )
		{
			const char* eq = ( const char* ) memchr( kp, '=', eol - kp );
			if ( eq )
				current.penetration_damage_mod = std::strtof( eq + 1, nullptr );
		}

		p = eol < cend ? eol + 1 : cend;
	}

	if ( !current.name.empty( ) )
		map.emplace( current.name, current );

	return map;
}

std::vector<surface_phys_t> c_autowall::load_map_materials( const std::filesystem::path vphys_path )
{
	std::vector<long long> map_hashes =
		extract_vphys_hashes( vphys_path.string( ) );

	const auto storage = download->storage_path( );

	const auto surface_txt =
		storage / "surface_data.txt";

	const auto surface_game_txt =
		storage / "surface_data_game.txt";

	auto hash_to_name =
		load_hash_to_name_map( surface_txt.string( ) );

	auto name_to_phys =
		load_name_to_phys_map( surface_game_txt.string( ) );


	std::vector<surface_phys_t> final_materials;
	final_materials.reserve( map_hashes.size( ) );
	for ( auto hash : map_hashes )
	{
		auto hn_it = hash_to_name.find( hash );
		if ( hn_it != hash_to_name.end( ) )
		{
			auto np_it = name_to_phys.find( hn_it->second );
			if ( np_it != name_to_phys.end( ) )
				final_materials.push_back( np_it->second );
			else
				final_materials.push_back( { hn_it->second, 1.0f, 1.0f } );
		}
		else
			final_materials.push_back( { "unknown", 1.0f, 1.0f } );
	}
	return final_materials;
}

void c_autowall::init_materials( )
{
	if (!download->ensure_storage( ))
	{
		log_dbg( "failed to prepare storage for materials" );
		return;
	}

	static std::string last_map;

	const std::string& level_name = global_vars->level_name( );
	if (level_name.empty( ))
		return;

	if ( level_name == last_map )
		return;

	last_map = level_name;         
	const std::string& current_map = last_map;

	const auto vphys_path = download->vphys_path( ) / ( current_map + ".vphys" );

	if ( !std::filesystem::exists( vphys_path ) )
	{
		log_dbg( ( "missing vphys for materials: " + current_map ).c_str( ) );
		return;
	}

	auto compiling = notif->begin_info( "compiling material meshes !!! visible check might not work, expect some temporary lags !!!" );
	if ( autowall_cache->load_from_cache( autowall->cached_materials, current_map ) )
	{
		notif->finish_success( compiling, "used internal material cache, have fun!" );
		return;
	}

	auto map_materials = autowall->load_map_materials( vphys_path );

	autowall->cached_materials.clear( );
	for ( size_t i = 0; i < map_materials.size( ); ++i )
		autowall->cached_materials[ static_cast< int >( i ) ] = map_materials[ i ];

	notif->finish_success( compiling, "finished compiling material meshes" );
	log_dbg( "materials initialized" );
}

bool c_autowall_material_cache::load_from_cache( std::unordered_map<int, surface_phys_t>& output, std::string map_name )
{
	auto hashed_name = FNV1A::Hash( map_name.data( ) );

	switch ( hashed_name )
	{
	case FNV1A::HashConst( "de_mirage" ):
		for ( auto& data : autowall_cache->de_mirage_cache )
			autowall->cached_materials[ data.first ] = data.second;

		return true;
		break;
	case FNV1A::HashConst( "de_inferno" ):
		for ( auto& data : autowall_cache->de_inferno_cache )
			autowall->cached_materials[ data.first ] = data.second;

		return true;
		break;
	case FNV1A::HashConst( "de_nuke" ):
		for ( auto& data : autowall_cache->de_nuke_cache )
			autowall->cached_materials[ data.first ] = data.second;

		return true;
		break;
	case FNV1A::HashConst( "de_dust2" ):
		for ( auto& data : autowall_cache->de_dust2_cache )
			autowall->cached_materials[ data.first ] = data.second;

		return true;
		break;
	default:
		return false;
		break;
	}
}
