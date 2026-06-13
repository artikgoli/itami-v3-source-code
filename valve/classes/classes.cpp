#include "players/c_base_weapon.h"
#include "../../app/features/entity_cache/entity_cache.h"

std::string c_entity_instance::get_schema_name()
{
	std::uintptr_t ent_identity = memory->read<std::uintptr_t>(reinterpret_cast<DWORD64>(this) + 0x10); //CEntityIdentity
	if (!ent_identity)
		return X("");

	std::uintptr_t ent_class_info = memory->read<std::uintptr_t>(ent_identity + 0x8); //CUtlSymbolLarge
	std::uintptr_t ent_class_info_data = memory->read<std::uintptr_t>(ent_class_info + 0x30);
	std::uintptr_t name_pointer = memory->read<std::uintptr_t>(ent_class_info_data + 0x8); //CScriptComponent

	std::string schema_name = memory->read_string(name_pointer);
	if (schema_name.empty())
		return X("");

	return schema_name;
}

std::string c_global_vars::level_name()
{
	const auto global_vars = memory->read<std::uintptr_t>( sdk->dw_global_vars.address );
	return memory->read_string(memory->read<std::uintptr_t>(global_vars + 0x188));
}

float c_global_vars::real_time( )
{
	const auto global_vars = memory->read<std::uintptr_t>( sdk->dw_global_vars.address );
	return memory->read<float>( global_vars + 0x0 );
}

float cs_weapon_info::get_inaccuracy( c_base_weapon* p_weapon, float max_speed )
{
	auto vdata = p_weapon->weapon_vdata();
	float force_spread = convar->get<float>("weapon_accuracy_forcespread");
	float air_spread_scale = convar->get<float>("weapon_air_spread_scale");
	float jump_impulse = convar->get<float>("sv_jump_impulse");


	if (force_spread > 0.0f)
		return force_spread;

	if (max_speed == 0.0f)
		max_speed = vdata->max_speed();

	float accuracy = p_weapon->acc_penalty();
	vector3d player_velocity = entity_cache->get_local_player().pawn->abs_velocity();
	float vertical_speed = std::abs(player_velocity.z);

	float m_inacc_scale = math::remap_value_clamped(player_velocity.length_2d(), max_speed * 0.34f, max_speed * 0.95f, 0.0f, 1.0f);

	if (m_inacc_scale > 0.0f)
	{
		if (!entity_cache->get_local_player().pawn->is_walking())
		{
			m_inacc_scale = powf(m_inacc_scale, 0.25f);
		}

		accuracy += m_inacc_scale * vdata->get_inacc_move( ( int ) vdata->weapon_type( ) );
	}

	uintptr_t ground_ent_addr = (uintptr_t)entity_helper->get_from_handle(entity_cache->get_local_player().pawn->ground_entity());
	if (!ground_ent_addr)
	{
		float inaccuracy_jump_initial = vdata->jump_initial() * air_spread_scale;
		float inaccuracy_jump_apex = vdata->jump_apex() * air_spread_scale;
		static const float max_falling_penalty = 2.0f;

		float sqrt_max_jump_speed = sqrtf(jump_impulse);
		float sqrt_vertical_speed = sqrtf(vertical_speed);

		float air_speed_inaccuracy = math::remap_value(sqrt_vertical_speed,
			sqrt_max_jump_speed * 0.25f,
			sqrt_max_jump_speed,
			inaccuracy_jump_apex,
			inaccuracy_jump_initial);

		if (air_speed_inaccuracy < 0)
			air_speed_inaccuracy = 0;
		else if (air_speed_inaccuracy >= 0.0f)
			air_speed_inaccuracy = fminf(inaccuracy_jump_initial * max_falling_penalty, air_speed_inaccuracy);

		accuracy += air_speed_inaccuracy;
	}

	return fminf(1.0, accuracy + p_weapon->turning_inacc());
}

float cs_weapon_info::inaccuracy_strafe()
{
	float inacc_bias = convar->get<float>("sv_strafing_inaccuracy_bias");
	float inacc_scale = convar->get<float>("sv_strafing_inaccuracy_scale");

	auto normalize_with_scale = [](float input_value, float scale_factor) -> float 
	{
		float clamped_value = 0.0f;
		float clamped_scale = FLT_MIN;

		if (input_value >= 0.0f)
			clamped_value = fminf(1.0f, input_value);

		if (scale_factor >= FLT_MIN)
			clamped_scale = fminf(1.0f, scale_factor);

		return clamped_value / ((((1.0f / clamped_scale) - 2.0f) * (1.0f - clamped_value)) + 1.0f);
	};

	vector3d player_velocity = entity_cache->get_local_player().pawn->abs_velocity();
	if (player_velocity.dot(player_velocity) < 0.001f)
		return 0.0f;

	vector3d eye_angle_dir = {};
	math::angle_vectors( entity_cache->get_local_player( ).pawn->eye_angles( ), eye_angle_dir );
	vector3d normalized_player_velocity = player_velocity.normalize_vector();

	float strafing_inacc = 1.0f - std::fabsf( normalized_player_velocity.dot( eye_angle_dir ) );

	float normalized_strafing_inacc = normalize_with_scale(strafing_inacc, inacc_bias); 
	normalized_strafing_inacc *= (player_velocity.length() / 250.0f) * inacc_scale;

	return normalized_strafing_inacc;
}