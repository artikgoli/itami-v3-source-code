#pragma once
#include <mutex>
#include <shared_mutex>

// in order to use it, you need to keep in mind
// the caching is done on a seperate thread
// meaning you cannot just access the player/entities from the rendering
// in order to actually use it, you need to work with the 'callback'
// each feature SHOULD have a on_entity_update function
// this function NEEDS to be called inside of entity_cache->update
// this way, you can be sure the data is in a safe state
// you can THEN copy the data you need, the vector's to your own class
// once you have a copy made, you can modify it or read from it at your will
// from the rendering thread (on_render)
// you COULD just access the cache anywhere
// but i dont recommend it as it can have delays/read access conflicts

// read access conflicts can be 'fixed' with mutex'es
// but this would introduce additional delays between
// waiting for the mutex to unlock, which is not great for rendering

// side notes:
//  because of how the update happens, you get access to 'global_time'
//  think of it as curtime in the engine
//  you can add delays between when stuff happens in your on_entity_update
//  eg. you can delay the aimbot activating based on that timer
// 
// - the timer gets compensated for every lost "tick"
// - the compensation re-runs the update function n-times, n being the number of 'ticks' missed/lost
// - the timer gets incremented by 10ms each tick
// - this means your on_entity_update will be called multiple times if the system is lagging behind
// - this also means you can use this timer for other things like animations, etc. although not recommended
// this system makes sure your function runs with a consistent frame rate / tick rate
// even if the full update takes 250ms to run, it will get compensated by running the update 25 times
// (this is good for aimbot since its gonna run the mouse_event aiming logic 25 times to compensate)
// this could also lead to some wierd things happening, like smoothing feeling like a snap instead
// optimize your code, try not to do heavy calculations in the update function
// you can put something like bsp parsing vischeck on another thread if needed
// then correctly sync the results back to the aimbot update function

struct c_cached_entity
{
	int index = {};
	int team_num = 0;

	c_base_entity* entity = {};
	c_player_controller* controller = {};
	c_player_pawn* pawn = {};
	c_game_scene_node* game_scene_node = {};
	c_weapon_services* weapon_service = {};

	bool is_valid( bool check_life = true ) const
	{
		if ( !entity || !controller || !pawn )
			return false;

		if ( check_life && ( pawn->get_health( ) <= 0 || pawn->life_state( ) != 256 ) )
			return false;

		return true;
	}
};

class c_entity_cache
{
public:
	struct pending_entity_t
	{
		c_base_entity* entity = nullptr;
		c_player_controller* controller = nullptr;
		c_player_pawn* pawn = nullptr;
		c_game_scene_node* game_scene_node = nullptr;
		int team_num = 0;
		int index = -1;
	};

	void cache_hitboxes( c_player_pawn* pawn );
	void update( const float& global_time );
	bool is_fresh_game_tick( );

	std::vector<pending_entity_t> build_pending_entities( int highest_index );

	const std::vector<c_cached_entity>& get_players( )
	{
		std::lock_guard guard( cache_mutex );
		return players;
	}

	const std::vector<c_cached_entity>& get_entities( )
	{
		std::lock_guard guard( cache_mutex );
		return entities;
	}

	const c_cached_entity& get_local_player( )
	{
		std::lock_guard guard( cache_mutex );
		return local_player;
	}

	struct cached_hitbox
	{
		int bone_index;
		vector3d min;
		vector3d max;
		float radius;
		int group_id;
		std::string name;
	};

	// jest w klasie
	// nie inline i nie static
	// bo just jest static

	std::unordered_map<c_player_pawn*, std::vector<cached_hitbox>> pawn_hitboxes{};

	float tickrate = 0.f;

private:

	std::mutex cache_mutex {};

	std::vector<c_cached_entity> players {};
	std::vector<c_cached_entity> entities {};
	c_cached_entity local_player {};

	float last_entity_cache_update = 0.f;
	float last_hitbox_cache_update = 0.f;
};

class c_entity_helper
{
public:
	c_base_entity* get_from_handle( c_base_handle handle );
	void batch_get_from_handles( std::vector<c_base_handle> handles, std::vector<c_base_entity*>& out_entities );

	template<typename T = c_base_entity>
	T* get_from_index( int index );
	int highest_entity_idx( );
};

inline auto entity_helper = std::make_unique<c_entity_helper>( );
inline auto entity_cache = std::make_unique<c_entity_cache>( );