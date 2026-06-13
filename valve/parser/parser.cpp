#include "parser.h"
#include "../../app/core/downloads/downloads.h"
#include "..\..\app\features\notifications\notifications.h"

void c_parser::save_bvh_recursive( bvh_node* node, std::ofstream& out )
{
	uint8_t is_valid = ( node != nullptr );
	out.write( reinterpret_cast< char* >( &is_valid ), sizeof( uint8_t ) );

	if ( !is_valid )
		return;

	out.write( reinterpret_cast< char* >( &node->bounds ), sizeof( aabb_t ) );

	uint8_t is_leaf = node->is_leaf( ) ? 1 : 0;
	out.write( reinterpret_cast< char* >( &is_leaf ), sizeof( uint8_t ) );

	if ( is_leaf )
	{
		uint64_t count = static_cast< uint64_t >( node->triangles.size( ) );
		out.write( reinterpret_cast< char* >( &count ), sizeof( uint64_t ) );

		if ( count > 0 )
			out.write( reinterpret_cast< char* >( node->triangles.data( ) ), count * sizeof( tri_parser ) );
	}
	else
	{
		save_bvh_recursive( node->left.get( ), out );
		save_bvh_recursive( node->right.get( ), out );
	}
}

std::unique_ptr<bvh_node> c_parser::load_bvh_recursive( std::ifstream& in )
{
	uint8_t is_valid = 0;
	in.read( reinterpret_cast< char* >( &is_valid ), sizeof( uint8_t ) );

	if ( in.fail( ) || !is_valid )
		return nullptr;

	auto node = std::make_unique<bvh_node>( );

	in.read( reinterpret_cast< char* >( &node->bounds ), sizeof( aabb_t ) );

	uint8_t is_leaf_val = 0;
	in.read( reinterpret_cast< char* >( &is_leaf_val ), sizeof( uint8_t ) );
	bool is_leaf = ( is_leaf_val == 1 );

	if ( is_leaf )
	{
		uint64_t count = 0;
		in.read( reinterpret_cast< char* >( &count ), sizeof( uint64_t ) );

		if ( count > 100000 )
		{
			log_dbg( "node parser failed, triangle count very high" );
			MessageBoxA( 0, "malformed triangle count\n let the developers know what map this it", "critical error", MB_OK );
			__debugbreak( );
			return nullptr;
		}

		node->triangles.resize( ( size_t ) count );
		if ( count > 0 ) {
			in.read( reinterpret_cast< char* >( node->triangles.data( ) ), count * sizeof( tri_parser ) );
		}
	}
	else
	{
		node->left = load_bvh_recursive( in );
		node->right = load_bvh_recursive( in );
	}
	return node;
}

std::unique_ptr<bvh_node> c_parser::build_bvh( std::vector<tri_parser>& tris )
{
	auto node = std::make_unique<bvh_node>( );
	if ( tris.empty( ) ) return node;

	node->bounds.minimum = node->bounds.maximum = tris[ 0 ].p1;
	for ( const auto& tri : tris )
	{
		for ( const auto& p : { tri.p1, tri.p2, tri.p3 } )
		{
			node->bounds.minimum.x = min( node->bounds.minimum.x, p.x );
			node->bounds.minimum.y = min( node->bounds.minimum.y, p.y );
			node->bounds.minimum.z = min( node->bounds.minimum.z, p.z );
			node->bounds.maximum.x = max( node->bounds.maximum.x, p.x );
			node->bounds.maximum.y = max( node->bounds.maximum.y, p.y );
			node->bounds.maximum.z = max( node->bounds.maximum.z, p.z );
		}
	}

	if ( tris.size( ) <= 4 ) {
		node->triangles = tris;
		return node;
	}

	vector3d size = node->bounds.maximum - node->bounds.minimum;
	int axis = ( size.x > size.y && size.x > size.z ) ? 0 : ( size.y > size.z ? 1 : 2 );

	std::sort( tris.begin( ), tris.end( ), [ axis ] ( const tri_parser& a, const tri_parser& b )
		{
			auto center = [ axis ] ( const tri_parser& t )
				{
					if ( axis == 0 ) return ( t.p1.x + t.p2.x + t.p3.x ) / 3.f;
					if ( axis == 1 ) return ( t.p1.y + t.p2.y + t.p3.y ) / 3.f;
					return ( t.p1.z + t.p2.z + t.p3.z ) / 3.f;
				};
			return center( a ) < center( b );
		} );

	size_t mid = tris.size( ) / 2;
	std::vector<tri_parser> left_vec( tris.begin( ), tris.begin( ) + mid );
	std::vector<tri_parser> right_vec( tris.begin( ) + mid, tris.end( ) );

	node->left = build_bvh( left_vec );
	node->right = build_bvh( right_vec );
	return node;
}

void c_parser::parse_materials( const std::string& array_content, std::vector<int>& materials )
{
	materials.clear( );
	std::istringstream iss( array_content );
	int num;
	while ( iss >> num )
	{
		materials.push_back( num );
		if ( iss.peek( ) == ',' || std::isspace( iss.peek( ) ) ) 
			iss.ignore( );
	}
}
std::vector<std::tuple<float, int, float, int>> c_parser::get_penetration_data( const vector3d& start, const vector3d& end )
{
	vector3d delta = end - start;
	float total_dist = start.dist( end );
	vector3d dir_inv = { 1.f / delta.x, 1.f / delta.y, 1.f / delta.z };

	std::vector<std::pair<float, int>> hits;
	std::function<void( bvh_node* )> traverse = [ & ] ( bvh_node* node )
		{
			if ( !node || !node->bounds.intersect( start, dir_inv, total_dist ) ) 
				return;

			if ( node->is_leaf( ) ) 
			{
				for ( const auto& tri : node->triangles ) 
				{
					float t = get_triangle_fraction( tri, start, end );
					if ( t > 0.0f && t < 1.0f ) 
						hits.push_back( { t * total_dist, tri.material_index } );
				}
			}
			else {
				traverse( node->left.get( ) );
				traverse( node->right.get( ) );
			}
		};

	traverse( root.get( ) );
	std::sort( hits.begin( ), hits.end( ) );

	std::vector<std::tuple<float, int, float, int>> result;

	for ( size_t i = 0; i < hits.size( ); )
	{
		float entry_t = hits[ i ].first;
		int entry_mat = hits[ i ].second;

		size_t next = i + 1;
		while ( next < hits.size( ) && ( hits[ next ].first - entry_t ) < 0.1f )
			next++;

		if ( next < hits.size( ) ) 
		{
			float exit_t = hits[ next ].first;

			if ( !result.empty( ) ) 
			{
				auto& last_hit = result.back( );
				float last_entry = std::get<0>( last_hit );
				float last_thickness = std::get<2>( last_hit );
				float last_exit = last_entry + last_thickness;

				if ( ( entry_t - last_exit ) < 10.f ) 
				{
					std::get<2>( last_hit ) = ( exit_t - last_entry );
					i = next + 1;
					continue;
				}
			}

			result.push_back( std::make_tuple( entry_t, entry_mat, exit_t - entry_t, hits[ next ].second ) );
			i = next + 1;
		}
		else 
		{
			float thickness = total_dist - entry_t;
			result.push_back( std::make_tuple( entry_t, entry_mat, thickness, entry_mat ) );
			break;
		}
	}
	return result;
}

float c_parser::get_triangle_fraction( const tri_parser& tri, const vector3d& start, const vector3d& end )
{
	const float EPSILON = 0.0000001f;
	vector3d edge1 = tri.p2 - tri.p1, edge2 = tri.p3 - tri.p1;
	vector3d ray_vec = end - start;
	vector3d h = ray_vec.cross( edge2 );
	float a = edge1.dot( h );
	if ( a > -EPSILON && a < EPSILON ) return 1.0f;

	float f = 1.0f / a;
	vector3d s = start - tri.p1;
	float u = f * s.dot( h );
	if ( u < 0.0f || u > 1.0f ) return 1.0f;

	vector3d q = s.cross( edge1 );
	float v = f * ray_vec.dot( q );
	if ( v < 0.0f || u + v > 1.0f ) return 1.0f;

	float t = f * edge2.dot( q );
	return ( t > EPSILON && t <= 1.0f ) ? t : 1.0f;
}

void c_parser::load_to_memory(const std::string map_name)
{
	parser_initialized = false;
	root.reset( );

	const auto cache_path = download->maps_path( ) / ( map_name + ".map" );
	std::ifstream in( cache_path, std::ios::binary );
	if ( !in.is_open( ) ) return;

	root = load_bvh_recursive( in );
	parser_initialized = ( root != nullptr );
}

void c_parser::load_map_data( unsigned int data_length, unsigned char* data )
{
	parser_initialized = false;

	root.reset( );
	triangles.clear( );

	size_t num_elements = data_length / sizeof( tri_parser );
	triangles.resize( num_elements );

	std::memcpy( triangles.data( ), data, data_length );

	if ( !triangles.empty( ) )
		root = build_bvh( triangles );

	if ( root )
		parser_initialized = true;

	std::vector<tri_parser>( ).swap( triangles );
}

std::string c_parser::read_file( const std::string& filename )
{
	std::ifstream file( filename );
	if ( !file.is_open( ) ) 
		return "";

	std::stringstream ss;
	ss << file.rdbuf( );
	return ss.str( );
}

void c_parser::process_hulls( const std::string& file_text )
{
	std::string hulls_block = find_block_by_name( file_text, "m_hulls" );
	if ( hulls_block.empty( ) ) 
		return;

	auto hull_structs = extract_structs( hulls_block );
	for ( const auto& hull_struct : hull_structs ) 
	{
		int surface_property = find_param( hull_struct, "m_nSurfacePropertyIndex" );
		std::string inner_hull = extract_inner_block( hull_struct, "m_Hull" );
		std::string bounds_block = extract_inner_block( inner_hull, "m_Bounds" );

		auto min_v = parse_float_array( extract_array_content( bounds_block, "m_vMinBounds" ) );
		auto max_v = parse_float_array( extract_array_content( bounds_block, "m_vMaxBounds" ) );

		if ( min_v.size( ) >= 3 && max_v.size( ) >= 3 )
		{
			auto tris = generate_triangles_from_aabb( 
				{ min_v[ 0 ], min_v[ 1 ], min_v[ 2 ] }, 
				{ max_v[ 0 ], max_v[ 1 ], max_v[ 2 ] }, surface_property );
			triangles.insert( triangles.end( ), tris.begin( ), tris.end( ) );
		}
	}
}

void c_parser::process_meshes( const std::string& file_text )
{
	std::string meshes_block = find_block_by_name( file_text, "m_meshes" );
	if ( meshes_block.empty( ) ) 
		return;

	auto mesh_structs = extract_structs( meshes_block );
	auto default_collision_indices = parse_collision_attributes( file_text );

	for ( const auto& mesh_struct : mesh_structs ) 
	{
		int collision_attr = find_param( mesh_struct, "m_nCollisionAttributeIndex" );

		if ( std::find( default_collision_indices.begin( ), default_collision_indices.end( ), collision_attr ) == default_collision_indices.end( ) )
			continue;

		int surface_property = find_param( mesh_struct, "m_nSurfacePropertyIndex" );
		std::string inner_mesh = extract_inner_block( mesh_struct, "m_Mesh" );

		std::string v_hex = extract_hex_blob( inner_mesh, "m_Vertices" );
		std::string t_hex = extract_hex_blob( inner_mesh, "m_Triangles" );
		std::string mat_content = extract_array_content( inner_mesh, "m_Materials" );

		auto v_bytes = hex_string_to_bytes( v_hex );
		auto t_bytes = hex_string_to_bytes( t_hex );

		std::vector<vector3d> verts( v_bytes.size( ) / 12 );
		std::memcpy( verts.data( ), v_bytes.data( ), v_bytes.size( ) );

		std::vector<int> materials;
		bool has_mat_array = !mat_content.empty( );
		if ( has_mat_array ) parse_materials( mat_content, materials );

		size_t num_tris = t_bytes.size( ) / 12;
		for ( size_t i = 0; i < num_tris; ++i )
		{
			int idx[ 3 ];
			std::memcpy( idx, &t_bytes[ i * 12 ], 12 );

			tri_parser tri;
			tri.p1 = verts[ idx[ 0 ] ];
			tri.p2 = verts[ idx[ 1 ] ];
			tri.p3 = verts[ idx[ 2 ] ];

			tri.material_index = has_mat_array ? ( ( i < materials.size( ) ) ? materials[ i ] : surface_property ) : surface_property;

			triangles.push_back( tri );
		}
	}
}

bool c_parser::save_map_cache( const std::filesystem::path& path )
{
	if ( !root ) 
		return false;
	std::ofstream out( path, std::ios::binary );
	if ( !out.is_open( ) ) 
		return false;

	save_bvh_recursive( root.get( ), out );
	return true;
}

bool c_parser::is_visible( const vector3d& start, const vector3d& end )
{
	if ( !parser_initialized || !root )
		return true;

	vector3d delta = end - start;
	float total_dist = delta.length( );

	if ( total_dist < 0.001f )
		return true;

	vector3d dir = delta / total_dist;
	vector3d dir_inv = { 1.f / dir.x, 1.f / dir.y, 1.f / dir.z };

	std::function<bool( bvh_node* )> check_collision = [ & ] ( bvh_node* node ) -> bool
		{
			if ( !node || !node->bounds.intersect( start, dir_inv, total_dist ) )
				return false;

			if ( node->is_leaf( ) ) 
			{
				for ( const auto& tri : node->triangles ) 
				{
					float t = get_triangle_fraction( tri, start, end );
					if ( t > 0.0f && t < 1.0f )
						return true;
				}
			}
			else 
			{
				if ( check_collision( node->left.get( ) ) ) return true;
				if ( check_collision( node->right.get( ) ) ) return true;
			}

			return false;
		};

	return !check_collision( root.get( ) );
}

bool c_parser::trace( const vector3d& start, const vector3d& end, vector3d& out_pos )
{
	if ( !parser_initialized || !root )
		return false;

	vector3d delta = end - start;
	float total_dist = delta.length( );
	if ( total_dist < 0.001f )
		return false;

	vector3d dir = delta / total_dist;
	vector3d dir_inv = { 1.f / dir.x, 1.f / dir.y, 1.f / dir.z };

	float best_t = 1.f;
	vector3d best_normal = {};

	std::function<void( bvh_node* )> traverse = [ & ] ( bvh_node* node )
		{
			if ( !node || !node->bounds.intersect( start, dir_inv, total_dist ) )
				return;

			if ( node->is_leaf( ) )
			{
				for ( const auto& tri : node->triangles )
				{
					constexpr float EPSILON = 0.0000001f;
					vector3d edge1 = tri.p2 - tri.p1;
					vector3d edge2 = tri.p3 - tri.p1;
					vector3d ray_vec = end - start;
					vector3d h = ray_vec.cross( edge2 );
					float a = edge1.dot( h );
					if ( a > -EPSILON && a < EPSILON )
						continue;

					float f = 1.f / a;
					vector3d s = start - tri.p1;
					float u = f * s.dot( h );
					if ( u < 0.f || u > 1.f )
						continue;

					vector3d q = s.cross( edge1 );
					float v = f * ray_vec.dot( q );
					if ( v < 0.f || u + v > 1.f )
						continue;

					float t = f * edge2.dot( q );
					if ( t > EPSILON && t < best_t )
					{
						best_t = t;
						best_normal = edge1.cross( edge2 );
						float len = best_normal.length( );
						if ( len > EPSILON )
							best_normal = best_normal / len;
					}
				}
			}
			else
			{
				traverse( node->left.get( ) );
				traverse( node->right.get( ) );
			}
		};

	traverse( root.get( ) );

	if ( best_t < 1.f )
	{
		out_pos = start + delta * best_t;
		// normal = best_normal;
		//   if ( normal.dot( dir ) > 0.f )
		//		normal = result.normal * -1.f;
		return true;
	}

	return false;
}

void c_parser::init( )
{
	if ( !download->ensure_storage( ) )
	{
		log_dbg( "failed to prepare storage content" );
		return;
	}

	const auto maps_dir = download->maps_path( );
	const auto vphys_dir = download->vphys_path( );

	SetFileAttributesW( maps_dir.c_str( ), FILE_ATTRIBUTE_HIDDEN );

	static FNV1A_t last_hash = 0;

	while ( true )
	{
		static std::string last_map;

		const std::string& level_name = global_vars->level_name( );
		if ( level_name.empty( ) )
		{
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
			continue;
		}

		if ( level_name == last_map )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
			continue;
		}

		last_map = level_name;
		current_map = level_name;

		const auto cache_path = maps_dir / ( current_map + ".map" );
		const auto source_path = vphys_dir / ( current_map + ".vphys" );

		map_parsed = false;
		parser_initialized = false;

		if ( std::filesystem::exists( cache_path ) )
		{
			log_dbg( ( "using cached map: " + cache_path.string( ) ).c_str( ) );
			load_to_memory( current_map );
			map_parsed = ( root != nullptr );
		}
		else if ( std::filesystem::exists( source_path ) )
		{
			log_dbg( ( "parsing raw physics data: " + source_path.string( ) ).c_str( ) );
			auto note = notif->begin_info( "generating raw map data (vis check won't work until its done) " );

			std::string file_text = read_file( source_path.string( ) );
			if ( !file_text.empty( ) )
			{
				triangles.clear( );
				root.reset( );

				process_meshes( file_text );
				process_hulls( file_text );

				if ( !triangles.empty( ) )
				{
					root = build_bvh( triangles );
					if ( root )
					{
						save_map_cache( cache_path );

						parser_initialized = true;
						map_parsed = true;

						std::vector<tri_parser>( ).swap( triangles );
						log_dbg( ( "successfully parsed map: " + current_map ).c_str( ) );
						notif->finish_success( note, "finished parsing map data" );
					}
				}
			}
		}
		else
		{
			log_dbg( ( "missing vphys and map data for: " + current_map ).c_str( ) );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	}
}

std::string c_parser::find_block_by_name( const std::string& text, const std::string& key )
{
	size_t pos = text.find( key );
	if ( pos == std::string::npos ) 
		return "";

	pos = text.find( '[', pos );
	if ( pos == std::string::npos ) 
		return "";

	int depth = 1;
	size_t start = pos + 1;
	size_t i = start;
	while ( i < text.size( ) && depth > 0 )
	{
		if ( text[ i ] == '[' ) 
			depth++;
		else if ( text[ i ] == ']' ) 
			depth--;
		i++;
	}
	return text.substr( start, i - start - 1 );
}

std::string c_parser::extract_inner_block( const std::string& block, const std::string& key )
{
	size_t pos = block.find( key );
	if ( pos == std::string::npos ) 
		return "";

	pos = block.find( '{', pos );
	if ( pos == std::string::npos ) 
		return "";

	int depth = 0;
	size_t start = pos;
	size_t i = pos;
	for ( ; i < block.size( ); ++i ) 
	{
		if ( block[ i ] == '{' ) 
			depth++;
		else if ( block[ i ] == '}' ) 
			depth--;

		if ( depth == 0 ) 
			break;
	}
	return block.substr( start, i - start + 1 );
}

std::vector<std::string> c_parser::extract_structs( const std::string& block )
{
	std::vector<std::string> structs;
	int depth = 0;
	size_t start = std::string::npos;
	for ( size_t i = 0; i < block.size( ); ++i )
	{
		if ( block[ i ] == '{' ) 
		{
			if ( depth == 0 ) 
				start = i;

			depth++;
		}
		else if ( block[ i ] == '}' ) 
		{
			depth--;
			if ( depth == 0 && start != std::string::npos ) 
			{
				structs.push_back( block.substr( start, i - start + 1 ) );
				start = std::string::npos;
			}
		}
	}
	return structs;
}

std::vector<unsigned char> c_parser::hex_string_to_bytes( const std::string& hex )
{
	std::vector<unsigned char> bytes;
	if ( hex.size( ) % 2 != 0 ) 
		return bytes;

	bytes.reserve( hex.size( ) / 2 );
	for ( size_t i = 0; i < hex.size( ); i += 2 ) 
	{
		std::string byte_str = hex.substr( i, 2 );
		unsigned int byte_val;
		std::istringstream iss( byte_str );
		iss >> std::hex >> byte_val;
		bytes.push_back( static_cast< unsigned char >( byte_val ) );
	}
	return bytes;
}

std::vector<float> c_parser::parse_float_array( const std::string& array_content )
{
	std::vector<float> values;
	std::istringstream iss( array_content );
	float num;
	while ( iss >> num ) 
	{
		values.push_back( num );
		if ( iss.peek( ) == ',' || std::isspace( iss.peek( ) ) )
			iss.ignore( );
	}
	return values;
}

std::vector<int> c_parser::parse_collision_attributes( const std::string& file_text )
{
	std::vector<int> default_indices;
	std::string collision_block = find_block_by_name( file_text, "m_collisionAttributes" );
	if ( collision_block.empty( ) ) return default_indices;

	auto structs = extract_structs( collision_block );
	for ( size_t idx = 0; idx < structs.size( ); idx++ ) 
	{
		std::string current_struct = structs[ idx ];
		std::transform( current_struct.begin( ), current_struct.end( ), current_struct.begin( ), ::tolower );

		if ( current_struct.find( "\"default\"" ) != std::string::npos )
			default_indices.push_back( static_cast< int >( idx ) );
	}
	return default_indices;
}

std::vector<tri_parser> c_parser::generate_triangles_from_aabb( const vector3d& min, const vector3d& max, int material_index )
{
	std::vector<tri_parser> tris;
	vector3d v[ 8 ] = {
		{min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, max.y, min.z}, {min.x, max.y, min.z},
		{min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, max.y, max.z}, {min.x, max.y, max.z}
	};

	auto add_face = [ & ] ( int i1, int i2, int i3, int i4 )
		{
			tris.push_back( { v[ i1 ], v[ i2 ], v[ i3 ], material_index } );
			tris.push_back( { v[ i1 ], v[ i3 ], v[ i4 ], material_index } );
		};

	add_face( 0, 1, 2, 3 );
	add_face( 4, 5, 6, 7 );
	add_face( 0, 1, 5, 4 );
	add_face( 3, 2, 6, 7 );
	add_face( 0, 3, 7, 4 );
	add_face( 1, 2, 6, 5 );

	return tris;
}

std::string c_parser::extract_hex_blob( const std::string& block, const std::string& key )
{
	size_t pos = block.find( key );
	if ( pos == std::string::npos )
		return "";

	pos = block.find( "#[", pos );
	if ( pos == std::string::npos )
		return "";

	pos += 2;

	size_t end = block.find( ']', pos );
	if ( end == std::string::npos )
		return "";

	std::string hex_str = block.substr( pos, end - pos );
	hex_str.erase( std::remove_if( hex_str.begin( ), hex_str.end( ), [ ] ( unsigned char c )
		{
			return std::isspace( c );
		} ), hex_str.end( ) );

	return hex_str;
}

std::string c_parser::extract_array_content( const std::string& block, const std::string& key )
{
	size_t pos = block.find( key );
	if ( pos == std::string::npos ) return "";
	pos = block.find( '[', pos );
	if ( pos == std::string::npos ) return "";
	size_t end = block.find( ']', pos );
	if ( end == std::string::npos ) return "";
	return block.substr( pos + 1, end - pos - 1 );
}

int c_parser::find_param( const std::string& block, const std::string& key )
{
	size_t pos = block.find( key );
	if ( pos == std::string::npos ) return -1;
	pos = block.find( "=", pos );
	if ( pos == std::string::npos ) return -1;
	pos++;
	while ( pos < block.size( ) && std::isspace( block[ pos ] ) ) pos++;
	std::istringstream iss( block.substr( pos ) );
	int value;
	iss >> value;
	return value;
}