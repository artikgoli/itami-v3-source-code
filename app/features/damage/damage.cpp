#include "damage.h"
#include "../entity_cache/entity_cache.h"
#include "../../renderer/window_handler.h"
#include "../../renderer/renderer.h"

void c_damage::on_render( )
{
    if (!hitmarker_enabled || !hitmarker.active)
        return;

    ULONGLONG elapsed = GetTickCount64( ) - hitmarker.start_time;

    if (elapsed >= 500)
    {
        hitmarker.active = false;
        return;
    }

    int alpha = elapsed <= 150 ? 255 : static_cast< int >( 255.f * ( 1.f - ( elapsed - 150 ) / 350.f ) );

    vector2d screen = window_handler->window_size;

    render->line( screen.x / 2 - 11, screen.y / 2 - 11, screen.x / 2 - 6, screen.y / 2 - 6, color( 255, 255, 255, alpha ), 1.2f );
    render->line( screen.x / 2 + 11, screen.y / 2 - 11, screen.x / 2 + 6, screen.y / 2 - 6, color( 255, 255, 255, alpha ), 1.2f );
    render->line( screen.x / 2 - 11, screen.y / 2 + 11, screen.x / 2 - 6, screen.y / 2 + 6, color( 255, 255, 255, alpha ), 1.2f );
    render->line( screen.x / 2 + 11, screen.y / 2 + 11, screen.x / 2 + 6, screen.y / 2 + 6, color( 255, 255, 255, alpha ), 1.2f );
}

void c_damage::on_input()
{
    enabled = config_variables->misc.hitsound->value;
    volume = config_variables->misc.hitsound_volume->value;
    hitmarker_enabled = config_variables->misc.hitmarker->value;
}

void c_damage::play_damage_sound()
{
    DWORD volume_level = volume * 0xFFFF / 110;
    waveOutSetVolume(nullptr, MAKELONG(volume_level, volume_level));
    PlaySoundA(reinterpret_cast<LPCSTR>(playertag_pickup_01), nullptr, SND_ASYNC | SND_MEMORY);
}

void c_damage::on_entity_update(const float& t)
{
    if (!enabled)
		return;

    static int previous_damage = 0;

    auto& local_player = entity_cache->get_local_player();
    auto& local_controller = local_player.controller;
    if ( !local_player.is_valid( ) || !local_controller )
        return;

    auto bullet = local_controller->action_tracking_service();
    if (!bullet)
        return;

    const int damage = bullet->total_damage();

    if (damage > previous_damage)
    {
        play_damage_sound();
        if (hitmarker_enabled)
        {
            hitmarker.active = true;
            hitmarker.start_time = GetTickCount64( );
        }
    }

    previous_damage = damage;
}

