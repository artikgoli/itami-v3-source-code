#pragma once
#include "../c_base_feature.h"
#include "sound.h"

class c_damage : public c_base_feature
{
public:
	void on_render() override;
	void on_input();
	void play_damage_sound();
    void on_entity_update(const float& t) override;
private:
	bool enabled = false;
	int volume = 50;
	bool hitmarker_enabled = false;

	struct hitmarker_t
	{
		bool active = false;
		ULONGLONG start_time = 0;
	};

	hitmarker_t hitmarker;
};
inline auto damage = std::make_unique<c_damage>();
