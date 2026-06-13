#pragma once
#include "../c_base_feature.h"
#include "../entity_cache/entity_cache.h"
#include "../../../valve/trace/trace.h"

class c_triggerbot : public c_base_feature
{
public:
	void on_render() override;
	void on_input();
	void on_entity_update(const float& t) override;

private:
	bool verify_last_target( const vector3d& eye_pos, const vector3d& end );
	bool is_hitbox_enabled( int hitbox );

	float shot_should_happen_at = 0.f;
	bool has_shot = false;
	int ammo_at_start = 0;

	void setup_weapon_config( );

	c_cached_entity last_best_player = {};

	bool is_active = false;
	bool delay = false;
	bool team_check = false;
	bool dynamic_delay = false;
	float static_delay_value = 0;
	float dynamic_delay_value = 0;	
	float next_attack_delay_value = 0;
	int hitchance = 0;
	int hitchance_accuracy = 0;

	bool autowall_enabled = false;
	int autowall_mindamage = 0;
	bool smoke_check = false;

	bool head = false;
	bool chest = false;
	bool pelvis = false;
	bool legs = false;
	bool toes = false;

	c_autowall_input autowall_input = {};
};

inline auto triggerbot = std::make_unique<c_triggerbot>();