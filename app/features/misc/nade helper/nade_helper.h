#pragma once
#include "../../c_base_feature.h"

struct nade_entry
{
	vector3d origin {};
	vector3d angle {};
	float max_origin_diff {};
	float max_angle_diff {};
	std::string name {};
	std::string throw_type {};
	int nade_type {};

	float   anim_range   = 0.f;
	float   anim_fov     = 0.f;
	float   anim_screen  = 0.f;
};

class c_nade_helper : public c_base_feature
{
public:
	void on_render( ) override;
	void on_entity_update( const float& t ) override;
	void on_input( );
	void save_entries_for_map( const std::string_view& map );
	void load_entries_for_map( const std::string_view& map );
	void remove_entry( int idx );

	const std::vector<nade_entry>& get_entries( ) const { return entries; }
	const std::string& get_current_map( ) const { return current_map; }

	void save_spot( );
	void export_to_clipboard( );
	void import_from_clipboard( );

private:
	void rebuild_spot_names( );
	void rebuild_typed_spot_lists( );

	std::mutex global_lock {};
	std::vector<nade_entry> entries {};
	std::vector<nade_entry> render_entries {};
	std::atomic<uint32_t>   entries_version { 0 };
	uint32_t                render_version  = UINT32_MAX;
	std::string current_map {};

	int prev_sel_flash   = 0;
	int prev_sel_he      = 0;
	int prev_sel_smoke   = 0;
	int prev_sel_molotov = 0;
	int prev_sel_decoy   = 0;
	int prev_sel_incend  = 0;

	vector3d origin   {};
	vector3d angle    {};
	int      item_def = 0;
	bool     has_nade = false;
	bool     auto_aim_active = false;
};

inline auto nade_helper = std::make_unique<c_nade_helper>( );
