#pragma once
#include "../c_base_feature.h"
#include "..\entity_cache\entity_cache.h"
#include "../../../valve/trace/trace.h"

class c_aimbot : public c_base_feature
{
public:

	void on_render( ) override;
	void setup_weapon_config( );
	void on_input( );
	bool verify_visibility( const vector3d& from, const vector3d& pos, const int hitbox_id ) const;
	vector3d predict_aimpoint( const vector3d& from, const vector3d& point, const vector3d& velocity );
	void find_best_target( const vector2d& screen_center, const vector3d& from, vector3d& output_pos, const vector3d& view_angles );
	void on_entity_update( const float& t ) override;
	void aim_at_safe( const vector3d& angle, vector2d* overshoot_vec, const float& smoothing_x_local, const float& smoothing_y_local );

	struct internal_config
	{
		float max_fov = 45.f;

		bool smooth_enabled = true;
		float smooth_x = 1.f;
		float smooth_y = 1.f;

		bool random_smooth_enabled = true;
		float randomize_smooth_x = 1.f;
		float randomize_smooth_y = 1.f;
		
		bool autowall_enabled = true;
		bool prediction = false;
		bool smoke_check = false;
		int smoothing_type = 0;
		float autowall_damage = 0.f;

		bool head {};
		bool chest {};
		bool pelvis {};
		bool legs {};
		bool toes {};
	} settings;
	
	bool is_active = false;

	c_autowall_input autowall_input = {};
	c_cached_entity locked_target {};
	float base_fov = 0.f;
};

inline auto aimbot = std::make_unique<c_aimbot>( );