#include "trace.h"
#include "../../app/features/entity_cache/entity_cache.h"
#include "..\..\app\features\autowall\autowall.h"

bool c_engine_trace::does_ray_hit_player( const ray_t& ray, trace_t* trace_out, const vector3d& min, const vector3d& max, 
	c_autowall_input info )
{
	vector3d end = ray.m_start + ( ray.m_direction * ray._distance );

	float t0, t1;

	OFFSET_FROM( skeleton_instance_offset, "CBodyComponentSkeletonInstance->m_skeletonInstance" );

	if ( !math::segment_aabb_intersect( ray.m_start, end, min, max, t0, t1 ) )
		return false;

	for ( auto& hitbox : entity_cache->pawn_hitboxes[ ray.target ] )
	{
		int bone_index = c_hitbox::get_bone_from_hitbox( hitbox.bone_index );

		if ( bone_index == -1 )
			continue;

		if ( hitbox.radius < 0.f || hitbox.radius > 100.f )
			continue;

		auto skeleton = ray.target->game_scene_node( )->skeleton_instance( );
		auto& model_state = skeleton->model_state( );

		uintptr_t model_state_addr = reinterpret_cast< uintptr_t >( &model_state );
		uintptr_t bone_array = memory->read<uintptr_t>( model_state_addr + skeleton_instance_offset );

		c_bone_data bone_data = memory->read<c_bone_data>( bone_array + ( uintptr_t ) bone_index * sizeof( c_bone_data ) );
		// i guess bro..

		matrix3x4_t bone_matrix = math::transform_to_matrix( &bone_data );

		vector3d min_transformed = math::matrix_transform( bone_matrix, hitbox.min );
		vector3d max_transformed = math::matrix_transform( bone_matrix, hitbox.max );

		vector3d hit_point;

		bool intersected = math::intersect_ray_capsule( ray.m_start, ray.m_direction,
			min_transformed, max_transformed, hitbox.radius * 0.85, &hit_point );

		if ( intersected )
		{
			if ( parser->parser_initialized )
			{
				info.scale_damage( info.internal_player, hitbox.bone_index );
				info.hitbox_id = hitbox.bone_index;

				if ( autowall->handle_bullet_penetration(hit_point, ray.m_start, 
					info.scaled_damage, info.range_modifier, info.penetration_power, info.hitbox_id ) > info.min_damage )
				{
					if ( trace_out )
						trace_out->m_hitbox_hit_index = hitbox.bone_index;

					return true;
				}
				else
				{
					if ( parser->is_visible( ray.m_start, hit_point ) )
					{
						if ( trace_out )
							trace_out->m_hitbox_hit_index = hitbox.bone_index;

						return true;
					}
				}

			}
			else
			{
				if ( trace_out )
					trace_out->m_hitbox_hit_index = hitbox.bone_index;

				return true;
			}
		}
	}

	return false;
}

bool c_engine_trace::does_ray_hit_player( const ray_t& ray, trace_t* trace_out,
	const vector3d& min, const vector3d& max, const bool& simplified )
{
	vector3d end = ray.m_start + (ray.m_direction * ray._distance);

	float t0, t1;

	OFFSET_FROM( skeleton_instance_offset, "CBodyComponentSkeletonInstance->m_skeletonInstance" );

	if ( !math::segment_aabb_intersect( ray.m_start, end, min, max, t0, t1 ) )
		return false;

	for ( auto& hitbox : entity_cache->pawn_hitboxes[ ray.target ] )
	{
		int bone_index = c_hitbox::get_bone_from_hitbox( hitbox.bone_index );

		if ( bone_index == -1 )
			continue;

		if ( hitbox.radius < 0.f || hitbox.radius > 100.f )
			continue;

		auto skeleton = ray.target->game_scene_node( )->skeleton_instance( );
		auto& model_state = skeleton->model_state( );

		uintptr_t model_state_addr = reinterpret_cast< uintptr_t >( &model_state );
		uintptr_t bone_array = memory->read<uintptr_t>( model_state_addr + skeleton_instance_offset );

		c_bone_data bone_data = memory->read<c_bone_data>( bone_array + ( uintptr_t ) bone_index * sizeof( c_bone_data ) );
		// i guess bro..

		matrix3x4_t bone_matrix = math::transform_to_matrix( &bone_data );

		vector3d min_transformed = math::matrix_transform( bone_matrix, hitbox.min );
		vector3d max_transformed = math::matrix_transform( bone_matrix, hitbox.max );

		vector3d hit_point;

		bool intersected = math::intersect_ray_capsule( ray.m_start, ray.m_direction,
			min_transformed, max_transformed, hitbox.radius * 0.85, &hit_point );

		if ( intersected )
		{
			if ( parser->parser_initialized && !simplified )
			{
				if ( parser->is_visible( ray.m_start, hit_point ) )
				{
					if ( trace_out )
						trace_out->m_hitbox_hit_index = hitbox.bone_index;

					return true;
				}
			}
			else
			{
				if ( trace_out )
					trace_out->m_hitbox_hit_index = hitbox.bone_index;

				return true;
			}
		}
	}

	return false;
}

bool c_engine_trace::hitchance( c_player_pawn* player, vector3d eye_position, vector3d vangles,
	float weapon_range, float weapon_inaccuracy, float weapon_spread, float recoil_index,
	int item_def_idx, int num_bullets, int hitchance, int hitchance_accuracy, bool autowall,
	uint32_t server_seed )
{
	if ( !hitchance )
		return true;

	const int SEED_MAX = 255 * ( static_cast< float >( hitchance_accuracy ) / 100.f );

	vector3d forward, right, up;
	math::angle_vectors( vangles, &forward, &right, &up );

	int total_hits = 0;
	int needed_hits = ( int ) std::ceilf( ( hitchance / 100.f ) * SEED_MAX );

	auto collision = ( ( c_base_entity* ) ( player ) )->collision( );
	if ( !collision )
		return false;

	vector3d mins = collision->mins( );
	vector3d maxs = collision->maxs( );

	mins += player->game_scene_node( )->origin( );
	maxs += player->game_scene_node( )->origin( );

	for ( int i = 0; i <= SEED_MAX; i++ )
	{
		vector2d offset = math::calc_spread( server_seed + i, weapon_inaccuracy, weapon_spread, recoil_index, item_def_idx, num_bullets );

		vector3d direction = ( forward + ( right * -offset.x ) + ( up * offset.y ) );
		direction.normalize_in_place( );

		ray_t ray {};
		ray.init( eye_position, eye_position + direction * weapon_range, player );

		if ( trace->does_ray_hit_player( ray, nullptr, mins, maxs, autowall ) )
			total_hits++;

		if ( total_hits >= needed_hits )
			return true;

		if ( ( SEED_MAX - i + total_hits ) < needed_hits )
			return false;
	}

	return false;
}

void c_autowall_input::init( c_base_weapon* weapon )
{
	autowall->get_weapon_data( weapon, this->base_damage, this->range_modifier, this->penetration_power, this->armor_modifier, this->headshot_multiplier );
	this->scaled_damage = 0.f;
}

void c_autowall_input::scale_damage( c_player_pawn* pawn, int hitbox )
{
	internal_player = pawn;
	if ( hitbox != -1 )
		scaled_damage = base_damage;

	const bool is_ct        = pawn->team_num( ) == 3;
	const bool headshot_only = convar->get<int>( "mp_damage_headshot_only" ) != 0;
	const float body_scale  = headshot_only ? 0.f : ( is_ct
		? convar->get<float>( "mp_damage_scale_ct_body" )
		: convar->get<float>( "mp_damage_scale_t_body" ) );
	const float head_scale  = is_ct
		? convar->get<float>( "mp_damage_scale_ct_head" )
		: convar->get<float>( "mp_damage_scale_t_head" );

	switch ( hitbox )
	{
	case 0:
		this->scaled_damage = this->base_damage * this->headshot_multiplier * head_scale;
		break;
	case HITBOX_PELVIS:
	case HITBOX_STOMACH:
		this->scaled_damage = this->base_damage * 1.25f * body_scale;
		break;
	case HITBOX_LEFT_FOOT:
	case HITBOX_RIGHT_FOOT:
		this->scaled_damage = this->base_damage * 0.75f * body_scale;
		break;
	default:
		this->scaled_damage = this->base_damage * body_scale;
		break;
	}

	if ( pawn->get_armor( ) <= 0 )
		return;

	float heavy_armor_bonus = 1.f;
	float armor_bonus = 0.5f;
	float armor_ratio = this->armor_modifier * 0.5f;

	// if has heavy armor // TODO
	// heavy_armor_bonus = 0.25
	// armor_bonus = 0.33
	// armor_ratio *= 0.20

	float damage_to_health = this->scaled_damage * armor_ratio;
	float damage_to_armor = ( this->scaled_damage - damage_to_health ) * ( heavy_armor_bonus * armor_bonus );
	if ( damage_to_armor > static_cast< float >( pawn->get_armor( ) ) )
		damage_to_health = this->scaled_damage - static_cast< float >( pawn->get_armor( ) ) / armor_bonus;

	this->scaled_damage = damage_to_health;
}