#pragma once

struct c_bone_data
{
	vector3d origin {};
	float scale {};
	vector4d rotation{};
	//float vector4d_temp[ 4 ] {}; // to pewnie to samo ale no
	// jak to to samo to wyjeb, to byl padding kurwa
};

class c_model_state
{
public:
	vector3d get_bone_pos( int bone )
	{
		OFFSET_FROM( skeleton_instance, "CBodyComponentSkeletonInstance->m_skeletonInstance" );

		uintptr_t offset = reinterpret_cast< uintptr_t >( this ) + skeleton_instance;
		uintptr_t bone_array = memory->read<uintptr_t>( offset );

		c_bone_data bone_data = memory->read<c_bone_data>( bone_array + ( uintptr_t ) bone * sizeof( c_bone_data ) );

		return bone_data.origin;
	}
	SCHEMA(std::uintptr_t, get_model_handle, "CModelState->m_hModel");
};

class c_skeleton_instance
{
public:
	c_model_state& model_state( )
	{
		OFFSET_FROM( model_state, "CSkeletonInstance->m_modelState" );
		return *( c_model_state* ) ( this + model_state );
	}
};

class c_game_scene_node
{
public:
	SCHEMA( bool, dormant, "CGameSceneNode->m_bDormant" );
	SCHEMA( vector3d, origin, "CGameSceneNode->m_vecOrigin" );
	SCHEMA( vector3d, abs_origin, "CGameSceneNode->m_vecAbsOrigin" );
	SCHEMA( vector3d, render_origin, "CGameSceneNode->m_vRenderOrigin" );
	SCHEMA( transform_4d, transform_ws, "CGameSceneNode->m_nodeToWorld" );

	c_skeleton_instance* skeleton_instance( )
	{
		return reinterpret_cast< c_skeleton_instance* > ( this );
	}
};