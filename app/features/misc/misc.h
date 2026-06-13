#pragma once
#include "../c_base_feature.h"
#include "../../renderer/renderer.h"

class c_misc : c_base_feature
{
public:
	void on_render( ) override;
	void on_entity_update( const float& t ) override;

	void dark_mode_flash( );
	void render_screen_dim( );
	void keybind_list( );

	void render_spread( );
	void render_spectator_list( );
	void render_bomb_timer( );
	void render_radar( );

	int kbl_w = 140;
	int kbl_h = 0;

	std::mutex entity_read_mutex {};

	float flash_alpha = 0.f;
	std::vector<std::string> spectators {};

	struct c_bomb_info
	{
		bool valid = false;
		bool ticking = false;
		bool defused = false;
		bool exploded = false;
		bool being_defused = false;
		int site = 0;
		float blow_time = 0.f;
		float timer_length = 0.f;
		float defuse_length = 0.f;
		float defuse_countdown = 0.f;
	};

	std::mutex bomb_mutex {};
	c_bomb_info bomb_info {};

	int bomb_w = 160;
	int bomb_h = 0;

	float cached_curtime = 0.f;
	int cached_tickcount = 0;

	std::atomic_bool is_in_game = false;

	struct c_radar_transform { float x = 0.f, y = 0.f, scale = 5.f; };
	struct c_radar_dot { vector3d origin {}; int team = 0; };

	std::mutex              radar_mutex {};
	std::vector<uint8_t>    radar_pending_rgba {};
	int                     radar_pending_w = 0;
	int                     radar_pending_h = 0;
	DXGI_FORMAT             radar_pending_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	c_radar_transform       radar_pending_transform {};
	std::string             radar_pending_map {};
	std::atomic<bool>       radar_has_pending { false };
	std::atomic<bool>       radar_needs_clear { false };
	std::string             radar_last_map {};  

	ID3D11ShaderResourceView* radar_srv = nullptr;
	int                  radar_tex_w = 0, radar_tex_h = 0;
	c_radar_transform    radar_transform {};
	std::string          radar_loaded_map {};

	std::vector<c_radar_dot> radar_dots {};
	int radar_local_team = 0;

	std::atomic<float> radar_local_x   { 0.f };
	std::atomic<float> radar_local_y   { 0.f };
	std::atomic<float> radar_local_yaw { 0.f };
	std::atomic<bool>  radar_local_valid { false };
};

inline auto misc = std::make_unique<c_misc>( );