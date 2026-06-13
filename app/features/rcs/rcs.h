#pragma once
#include "../c_base_feature.h"

class c_rcs : public c_base_feature
{
public:
	void on_render( ) override
	{

	}
	vector3d get_punch( bool change_from_cfg );
	void on_entity_update( const float& t ) override;
	void setup_weapon_config( );

	bool enabled = false;
	float compensate_x = 0.f;
	float compensate_y = 0.f;
	float smooth_x = 0.f;
	float smooth_y = 0.f;
};

inline auto rcs = std::make_unique<c_rcs>( );