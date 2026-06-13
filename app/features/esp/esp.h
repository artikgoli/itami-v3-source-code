#pragma once

struct rect_t
{
	float x {}, y {}, w {}, h {};
	rect_t( ) = default;
};

class c_player_pawn;

struct esp_animation_data
{
	vector3d origin = {};
	vector3d maxs = {};
	std::array<vector3d, 28> bone_pos = {};
	std::array<vector4d, 28> bone_rot = {};
	c_player_pawn* pawn = nullptr;

	float health_displayed = 0.f;
	float health_target = 0.f;
	float health_start = 0.f;
	float health_anim_time = 0.f;
	int max_health = 100;

	float global_alpha = 0.f;
	float visibility_alpha = 1.f;
	bool is_valid = false;
	bool was_valid = false;

	std::wstring name_w = {};
	std::string weapon_display_name = {};
	std::size_t hashed_name = 0;
	std::size_t hashed_weapon_name = 0;
	int clip_1 = -1;
	int max_clip = 0;
	bool reloading = false;
	float ammo_displayed = 0.f;
	float ammo_target = 0.f;
	float ammo_start = 0.f;
	float ammo_anim_time = 0.f;

	int team = 0;
	int armor = 0;
	bool helmet = false;
	bool kit = false;
	bool scoped = false;
	bool blind = false;
	bool defusing = false;
	bool has_bomb = false;
	bool is_visible = false;

	void update_ammo( int new_clip, int new_max_clip, float cur_time )
	{
		if ( new_clip < 0 || new_max_clip <= 0 )
			return;
		float new_ratio = std::clamp( ( float ) new_clip / ( float ) new_max_clip, 0.f, 1.f );
		if ( new_ratio != ammo_target )
		{
			ammo_start = ammo_displayed;
			ammo_target = new_ratio;
			ammo_anim_time = cur_time;
		}
		constexpr float DURATION = 0.15f;
		float t = std::clamp( ( cur_time - ammo_anim_time ) / DURATION, 0.f, 1.f );
		t = 1.f - ( 1.f - t ) * ( 1.f - t );
		ammo_displayed = ammo_start + ( ammo_target - ammo_start ) * t;
	}

	void update_health( int new_health, int new_max_health, float cur_time )
	{
		if ( new_max_health > 0 )
			max_health = new_max_health;
		if ( new_health != ( int ) health_target )
		{
			health_start = health_displayed;
			health_target = static_cast< float >( new_health );
			health_anim_time = cur_time;
		}
		constexpr float DURATION = 0.25f;
		float t = std::clamp( ( cur_time - health_anim_time ) / DURATION, 0.f, 1.f );
		t = 1.f - ( 1.f - t ) * ( 1.f - t );
		health_displayed = health_start + ( health_target - health_start ) * t;
	}

	bool update_alpha( float frame_time, float cur_time )
	{
		if ( is_valid )
		{
			global_alpha = std::clamp( global_alpha + frame_time * 6.f, 0.f, 1.f );
			return false;
		}

		health_target = 0.f;

		constexpr float DURATION = 0.25f;
		float t = std::clamp( ( cur_time - health_anim_time ) / DURATION, 0.f, 1.f );

		t = 1.f - ( 1.f - t ) * ( 1.f - t );

		health_displayed = health_start + ( health_target - health_start ) * t;
		global_alpha = std::clamp( global_alpha - frame_time * 3.f, 0.f, 1.f );
		return global_alpha <= 0.f;
	}

	void update_visibility( float frame_time, bool vis_only, float max_invis_alpha )
	{
		if ( !vis_only )
		{
			visibility_alpha = 1.f;
			return;
		}

		float target = is_visible ? 1.f : max_invis_alpha;
		visibility_alpha = std::clamp( visibility_alpha + ( target - visibility_alpha ) * frame_time * 4.f, 0.f, 1.f );
	}
};

class c_esp
{
public:
	void init( );

	struct c_cached_entity
	{
		vector3d mins = {};
		vector3d maxs = {};
		vector3d origin = {};
		transform_4d transform = {};
		e_entity_type type = e_entity_type::unknown;
		bool is_player = false;
		bool use_transform = false;
		bool timer_active = false;
		int timer_tick_begin = 0;
		float timer_duration = 0.f;
		int entity_index = 0;
		std::string name = {};
		c_cached_entity( ) = default;
	};

	struct c_cached_player : public c_cached_entity
	{
		std::array<vector3d, 28> bone_pos = {};
		std::array<vector4d, 28> bone_rot = {};
		c_player_pawn* pawn = nullptr;
		int health = 0;
		int max_health = 0;
		bool damage_immune = false;
		bool helmet = false;
		bool kit = false;
		int armor = 0;
		bool scoped = false;
		bool defusing = false;
		bool blind = false;
		bool has_bomb = false;
		bool is_visible = false;
		int team = 0;
		std::string weapon_name = {};
		std::size_t weapon_name_hash = 0;
		uint32_t weapon_name_fnv = 0;
		std::size_t name_hash = 0;
		int clip_1 = -1;
		int max_clip = 0;
		bool reloading = false;
	};

	void cache_players( );
	void cache_entities( );
	void render_entities( );
	void render_players( );

	void on_input( );

	std::mutex is_enabled_mutex {};
	bool is_enabled = false;

private:
	bool calc_bbox( const c_cached_entity& ent, rect_t& out );

	void draw_healthbar( const esp_animation_data& anim, const rect_t& box, float alpha );
	void draw_weapon( const esp_animation_data& anim, const rect_t& box, float alpha );
	void draw_skeleton( const esp_animation_data& anim, float distance, float alpha );
	void draw_head_indicator( const esp_animation_data& anim, float alpha );
	void draw_flags( const esp_animation_data& anim, const rect_t& box, float alpha );
	void draw_oof( const c_cached_player& entity, const vector3d& local_pos, const vector3d& view_angles, float cur_time );

public:
	c_cached_player player_buf[ 2 ][ 65 ] = {};
	c_cached_entity entity_buf[ 2 ][ 4096 ] = {};
	unsigned int player_buf_size[ 2 ] = {};
	unsigned int entity_buf_size[ 2 ] = {};
	std::atomic<int> player_front { 0 };
	std::atomic<int> entity_front { 0 };
	std::atomic<int> planted_bomb_idx { -1 };

	std::unordered_map<int, esp_animation_data> animation_data {};
};

inline auto esp = std::make_unique<c_esp>( );