#pragma once

struct surface_phys_t
{
	std::string name {};
	float penetration_distance_mod = 1.0f;
	float penetration_damage_mod = 1.0f;
	float same_mat_pen_override = 0.f;

	surface_phys_t( ) = default;

	surface_phys_t( const std::string& name, float penetration_distance_mod, float penetration_damage_mod )
		: name( name ), penetration_distance_mod( penetration_distance_mod ), penetration_damage_mod( penetration_damage_mod )
	{
	}
};

class c_autowall
{
public:
	void get_weapon_data( c_base_weapon* weapon, float& base_damage, float& range_modifier, float& pen_power, float& armor_modifier, float& headshot_mult );
	float handle_bullet_penetration( const vector3d& from, const vector3d& to, const float& base_damage,
		const float& range_modifier, const float& penetration_power, const int& hitbox );

	std::vector<long long> extract_vphys_hashes( const std::string& path );
	std::unordered_map<long long, std::string> load_hash_to_name_map( const std::string& path );
	std::unordered_map<std::string, surface_phys_t> load_name_to_phys_map( const std::string& path );
	std::vector<surface_phys_t> load_map_materials( const std::filesystem::path vphys_path );

	void init_materials( );

	bool finished_material_comp = false;
	std::unordered_map<int, surface_phys_t> cached_materials {};
};

inline auto autowall = std::make_unique<c_autowall>( );

class c_autowall_material_cache
{
public:
	static inline std::unordered_map<int, surface_phys_t> de_mirage_cache =
	{
			{ 0,  { "Wood_Plank",        0.850000024f, 1.00000000f } },
			{ 1,  { "metal",             0.400000006f, 1.00000000f } },
			{ 2,  { "dirt",              0.600000024f, 0.300000012f } },
			{ 3,  { "metalvehicle",      0.500000000f, 1.00000000f } },
			{ 4,  { "glass",             0.990000010f, 1.00000000f } },
			{ 5,  { "metalpanel",        0.500000000f, 0.449999988f } },
			{ 6,  { "solidmetal",        0.270000011f, 0.300000012f } },
			{ 7,  { "rock",              1.00000000f,  0.250000000f } },
			{ 8,  { "Wood",              0.899999976f, 0.600000024f } },
			{ 9,  { "Wood_Panel",        1.00000000f,  1.00000000f } },
			{ 10, { "cardboard",         0.949999988f, 0.990000010f } },
			{ 11, { "concrete",          0.500000000f, 0.250000000f } },
			{ 12, { "boulder",           1.00000000f,  1.00000000f } },
			{ 13, { "pottery",           0.949999988f, 0.600000024f } },
			{ 14, { "Wood_Solid",        0.800000012f, 1.00000000f } },
			{ 15, { "default",           0.500000000f, 0.500000000f } },
			{ 16, { "no_decal",           1.00000000f,  1.00000000f } },
			{ 17, { "metal_barrel",      0.00999999978f, 0.00999999978f } },
			{ 18, { "plaster",           0.699999988f, 0.600000024f } },
			{ 19, { "upholstery",        0.750000000f, 1.00000000f } },
			{ 20, { "Wood_Furniture",    1.00000000f,  1.00000000f } },
			{ 21, { "chainlink",         0.990000010f, 0.990000010f } },
			{ 22, { "brick",             0.469999999f, 0.300000012f } },
			{ 23, { "carpet",            0.750000000f, 1.00000000f } },
			{ 24, { "metal_sand_barrel", 0.00999999978f, 0.00999999978f } },
			{ 25, { "Plastic_Box",       0.750000000f, 1.00000000f } },
			{ 26, { "rubber",            0.850000024f, 0.500000000f } },
			{ 27, { "computer",          0.400000006f, 0.449999988f } },
			{ 28, { "metalvent",         0.600000024f, 0.449999988f } },
			{ 29, { "Wood_Basket",       0.899999976f, 1.00000000f } },
			{ 30, { "Wood_Tree",         1.00000000f,  1.00000000f } },
			{ 31, { "Wood_Solid",        0.800000012f, 1.00000000f } },
			{ 32, { "Wood",              0.899999976f, 0.600000024f } },
			{ 33, { "Wood_Plank",        0.850000024f, 1.00000000f } },
			{ 34, { "ladder",            1.00000000f,  1.00000000f } },
			{ 35, { "sand",              0.300000012f, 0.250000000f } },
			{ 36, { "metal",             0.400000006f, 1.00000000f } },
			{ 37, { "tile",              0.699999988f, 0.300000012f } },
			{ 38, { "Wood_Ladder",       0.899999976f, 1.00000000f } },
			{ 39, { "sheetrock",         0.850000024f, 0.600000024f } },
	};

	static inline std::unordered_map<int, surface_phys_t> de_inferno_cache =
	{
		{0, {"metal", 0.400000006f, 1.00000000f}},
		{1, {"concrete", 0.500000000f, 0.250000000f}},
		{2, {"rock", 1.00000000f, 0.250000000f}},
		{3, {"metalrailing", 0.00000000f, 1.00000000f}},
		{4, {"Wood", 0.899999976f, 0.600000024f}},
		{5, {"paper", 1.00000000f, 1.00000000f}},
		{6, {"Metal_Box", 0.500000000f, 1.00000000f}},
		{7, {"plastic", 1.00000000f, 1.00000000f}},
		{8, {"concrete_block", 1.00000000f, 1.00000000f}},
		{9, {"glass", 0.990000010f, 1.00000000f}},
		{10, {"default", 0.500000000f, 0.500000000f}},
		{11, {"glassbottle", 0.990000010f, 1.00000000f}},
		{12, {"computer", 0.400000006f, 0.449999988f}},
		{13, {"Cloth", 1.00000000f, 1.00000000f}},
		{14, {"papercup", 1.00000000f, 1.00000000f}},
		{15, {"Wood_Dense", 0.500000000f, 0.300000012f}},
		{16, {"canister", 1.00000000f, 1.00000000f}},
		{17, {"Wood_Panel", 1.00000000f, 1.00000000f}},
		{18, {"plastic_milkCrate", 1.00000000f, 1.00000000f}},
		{19, {"porcelain", 0.949999988f, 1.00000000f}},
		{20, {"rubber", 0.850000024f, 0.500000000f}},
		{21, {"Wood_Furniture", 1.00000000f, 1.00000000f}},
		{22, {"metal_sheetmetal", 1.00000000f, 1.00000000f}},
		{23, {"concrete_polished", 1.00000000f, 1.00000000f}},
		{24, {"Wood_Basket", 0.899999976f, 1.00000000f}},
		{25, {"brass_bell_large", 1.00000000f, 1.00000000f}},
		{26, {"brass_bell_medium", 1.00000000f, 1.00000000f}},
		{27, {"brass_bell_small", 1.00000000f, 1.00000000f}},
		{28, {"brass_bell_smallest", 1.00000000f, 1.00000000f}},
		{29, {"brass_bell_smallest_g", 1.00000000f, 1.00000000f}},
		{30, {"Wood_Plank", 0.850000024f, 1.00000000f}},
		{31, {"plasticbottle", 1.00000000f, 1.00000000f}},
		{32, {"plaster", 0.699999988f, 0.600000024f}},
		{33, {"tile", 0.699999988f, 0.300000012f}},
		{34, {"Wood_Solid", 0.800000012f, 1.00000000f}},
		{35, {"gravel", 0.400000006f, 1.00000000f}},
		{36, {"grass", 1.00000000f, 1.00000000f}},
		{37, {"metalvehicle", 0.500000000f, 1.00000000f}},
		{38, {"rubbertire", 1.00000000f, 1.00000000f}},
		{39, {"cardboard", 0.949999988f, 0.990000010f}},
		{40, {"metal_dumpster", 1.00000000f, 1.00000000f}},
		{41, {"metalpanel", 0.500000000f, 0.449999988f}},
		{42, {"blockbullets", 0.00999999978f, 0.00100000005f}},
		{43, {"mud", 1.00000000f, 1.00000000f}}
	};

	static inline std::unordered_map<int, surface_phys_t> de_nuke_cache = {
		{0, {"Metal_Box", 0.500000000f, 1.00000000f}},
		{1, {"metal", 0.400000006f, 1.00000000f}},
		{2, {"metalvent", 0.600000024f, 0.449999988f}},
		{3, {"metalrailing", 0.00000000f, 1.00000000f}},
		{4, {"chainlink", 0.990000010f, 0.990000010f}},
		{5, {"glass", 0.990000010f, 1.00000000f}},
		{6, {"plastic", 1.00000000f, 1.00000000f}},
		{7, {"concrete", 0.500000000f, 0.250000000f}},
		{8, {"metalvehicle", 0.500000000f, 1.00000000f}},
		{9, {"metal_barrel", 0.00999999978f, 0.00999999978f}},
		{10, {"carpet", 0.750000000f, 1.00000000f}},
		{11, {"rubber", 0.850000024f, 0.500000000f}},
		{12, {"metalgrate", 0.949999988f, 0.990000010f}},
		{13, {"default", 0.500000000f, 0.500000000f}},
		{14, {"tile", 0.699999988f, 0.300000012f}},
		{15, {"Wood", 0.899999976f, 0.600000024f}},
		{16, {"metalpanel", 0.500000000f, 0.449999988f}},
		{17, {"concrete_polished", 1.00000000f, 1.00000000f}},
		{18, {"ladder", 1.00000000f, 1.00000000f}},
		{19, {"grass", 1.00000000f, 1.00000000f}},
		{20, {"gravel", 0.400000006f, 1.00000000f}},
		{21, {"glassfloor", 0.990000010f, 1.00000000f}},
		{22, {"rubbertire", 1.00000000f, 1.00000000f}},
		{23, {"blockbullets", 0.00999999978f, 0.00100000005f}},
		{24, {"audioblocker", 1.00000000f, 1.00000000f}},
		{25, {"asphalt", 0.550000012f, 0.300000012f}},
		{26, {"water", 0.300000012f, 1.00000000f}},
		{27, {"dirt", 0.600000024f, 0.300000012f}}
	};

	static inline std::unordered_map<int, surface_phys_t> de_dust2_cache = {
		{0, {"concrete", 0.500000000f, 0.250000000f}},
		{1, {"Wood", 0.899999976f, 0.600000024f}},
		{2, {"chainlink", 0.990000010f, 0.990000010f}},
		{3, {"Wood_Panel", 1.00000000f, 1.00000000f}},
		{4, {"metal_sand_barrel", 0.00999999978f, 0.00999999978f}},
		{5, {"glass", 0.990000010f, 1.00000000f}},
		{6, {"metal", 0.400000006f, 1.00000000f}},
		{7, {"rock", 1.00000000f, 0.250000000f}},
		{8, {"dirt", 0.600000024f, 0.300000012f}},
		{9, {"default", 0.500000000f, 0.500000000f}},
		{10, {"carpet", 0.750000000f, 1.00000000f}},
		{11, {"solidmetal", 0.270000011f, 0.300000012f}},
		{12, {"metalpanel", 0.500000000f, 0.449999988f}},
		{13, {"plastic", 1.00000000f, 1.00000000f}},
		{14, {"Wood_Dense", 0.500000000f, 0.300000012f}},
		{15, {"Wood_Plank", 0.850000024f, 1.00000000f}},
		{16, {"metalrailing", 0.00000000f, 1.00000000f}},
		{17, {"computer", 0.400000006f, 0.449999988f}},
		{18, {"metal_dumpster", 1.00000000f, 1.00000000f}},
		{19, {"rubbertire", 1.00000000f, 1.00000000f}},
		{20, {"metalvehicle", 0.500000000f, 1.00000000f}},
		{21, {"metalvent", 0.600000024f, 0.449999988f}},
		{22, {"Wood_Crate", 0.899999976f, 1.00000000f}},
		{23, {"Wood_Basket", 0.899999976f, 1.00000000f}},
		{24, {"pottery", 0.949999988f, 0.600000024f}},
		{25, {"Wood_Solid", 0.800000012f, 1.00000000f}},
		{26, {"tile", 0.699999988f, 0.300000012f}},
		{27, {"sand", 0.300000012f, 0.250000000f}},
		{28, {"Wood_Tree", 1.00000000f, 1.00000000f}},
		{29, {"gravel", 0.400000006f, 1.00000000f}},
		{30, {"Wood", 0.899999976f, 0.600000024f}}
	};

	bool load_from_cache( std::unordered_map<int, surface_phys_t>& output, std::string map_name );
};

inline auto autowall_cache = std::make_unique<c_autowall_material_cache>( );