#pragma once

struct ray_t
{
public:
	vector3d m_start {};
	vector3d m_direction {};
	float _distance {};
	c_player_pawn* target {};

	void init( const vector3d& start, const vector3d& end, c_player_pawn* pawn )
	{
		m_start = start;
		m_direction = ( end - start ).normalize_vector( );
		_distance = ( end - start ).length( );
		target = pawn;
	}

	void init( const vector3d& start, const vector3d& end, c_player_pawn* pawn, const vector3d& direction )
	{
		m_start = start;
		m_direction = direction;
		_distance = ( end - start ).length( );
		target = pawn;
	}

	ray_t( ) = default;
};

struct trace_t
{
public:
	int m_hitbox_hit_index = hitboxes_t::HITBOX_INVALID;
};

struct c_autowall_input
{
	float base_damage = 0.f;
	float range_modifier = 0.f;
	float penetration_power = 0.f;
	float armor_modifier = 0.f;
	float headshot_multiplier = 0.f;

	float scaled_damage = 0.f;
	int hitbox_id = 0;
	float min_damage = 0.f;

	c_player_pawn* internal_player = {};

	void init( c_base_weapon* weapon );
	void scale_damage( c_player_pawn* pawn, int hitbox );
};

class c_engine_trace {
public:
	bool does_ray_hit_player( const ray_t& ray, trace_t* trace_out, const vector3d& min, const vector3d& max,
		c_autowall_input info );

	bool does_ray_hit_player( const ray_t& ray, trace_t* trace_out, const vector3d& min, const vector3d& max, const bool& simplified );
	bool hitchance( c_player_pawn* player, vector3d eye_position, vector3d vangles, float weapon_range, float weapon_inaccuracy, float weapon_spread, float recoil_index, int item_def_idx, int num_bullets, int hitchance, int hitchance_accuracy, bool autowall, uint32_t server_seed );
};
inline auto trace = std::make_unique<c_engine_trace>();