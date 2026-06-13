#pragma once

struct tri_parser 
{
	vector3d p1, p2, p3;
	int material_index;
};

struct aabb_t
{
	vector3d minimum, maximum;

	bool intersect( const vector3d& origin, const vector3d& dir_inv, float total_dist ) const
	{
		float t1 = ( minimum.x - origin.x ) * dir_inv.x;
		float t2 = ( maximum.x - origin.x ) * dir_inv.x;
		float t3 = ( minimum.y - origin.y ) * dir_inv.y;
		float t4 = ( maximum.y - origin.y ) * dir_inv.y;
		float t5 = ( minimum.z - origin.z ) * dir_inv.z;
		float t6 = ( maximum.z - origin.z ) * dir_inv.z;

		float tmin = max( max( min( t1, t2 ), min( t3, t4 ) ), min( t5, t6 ) );
		float tmax = min( min( max( t1, t2 ), max( t3, t4 ) ), max( t5, t6 ) );

		if ( tmax < 0 || tmin > tmax ) return false;
		if ( tmin > total_dist ) return false;

		return true;
	}
};

struct bvh_node
{
	aabb_t bounds;
	std::unique_ptr<bvh_node> left, right;
	std::vector<tri_parser> triangles;

	bool is_leaf( ) const
	{
		return !left && !right;
	}
};

class c_parser 
{
public:

	static inline std::string current_map;

	bool parser_initialized = false;
	static inline bool map_parsed = false;

	std::string extract_array_content( const std::string& block, const std::string& key );
	int find_param( const std::string& block, const std::string& key );
	void save_bvh_recursive( bvh_node* node, std::ofstream& out );
	std::unique_ptr<bvh_node> load_bvh_recursive( std::ifstream& in );
	std::unique_ptr<bvh_node> build_bvh( std::vector<tri_parser>& tris );
	void parse_materials( const std::string& array_content, std::vector<int>& materials );
	std::vector<std::tuple<float, int, float, int>> get_penetration_data( const vector3d& start, const vector3d& end );
	float get_triangle_fraction( const tri_parser& tri, const vector3d& start, const vector3d& end );
	void load_to_memory(std::string map_name);
	void load_map_data(unsigned int data_length, unsigned char* data);

	std::string read_file( const std::string& filename );
	void process_hulls( const std::string& file_text );
	void process_meshes( const std::string& file_text );
	bool save_map_cache( const std::filesystem::path& path );
	bool is_visible( const vector3d& start, const vector3d& end );
	bool trace( const vector3d& start, const vector3d& end, vector3d& out_pos );
	void init();

	std::string find_block_by_name( const std::string& text, const std::string& key );
	std::string extract_inner_block( const std::string& block, const std::string& key );
	std::vector<std::string> extract_structs( const std::string& block );
	std::vector<unsigned char> hex_string_to_bytes( const std::string& hex );
	std::vector<float> parse_float_array( const std::string& array_content );
	std::vector<int> parse_collision_attributes( const std::string& file_text );
	std::vector<tri_parser> generate_triangles_from_aabb( const vector3d& min, const vector3d& max, int material_index );
	std::string extract_hex_blob( const std::string& block, const std::string& key );

	bool file_exists(const std::wstring& path) {
		return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
	}

	bool download_exists = false;

private:
	std::unique_ptr<bvh_node> root = nullptr;
	std::vector<tri_parser> triangles;
};
inline auto parser = std::make_shared<c_parser>();