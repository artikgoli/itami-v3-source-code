#pragma once

class c_base_feature
{
public:
	virtual void on_render( ) = 0;
	virtual void on_entity_update( const float& t ) = 0;
};
