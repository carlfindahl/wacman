#include "audio_system.h"
#include "components.h"
#include "events.h"
#include "audio/sound_manager.h"

namespace pac
{
extern entt::dispatcher g_event_queue;

AudioSystem::AudioSystem(entt::registry& reg) : System(reg)
{
    m_pickup_conn = g_event_queue.sink<EvPacPickup>().connect<&AudioSystem::recieve_pickup>(*this);
    m_ghost_conn = g_event_queue.sink<EvGhostStateChanged>().connect<&AudioSystem::recieve_ghost_state>(*this);
    m_pacman_conn = g_event_queue.sink<EvPacLifeChanged>().connect<&AudioSystem::recieve_pacman_life>(*this);
}

void AudioSystem::update(float dt) {}

void AudioSystem::recieve_pickup(const EvPacPickup& pickup) const
{
    if (pickup.pickup_name != "food")
    {
        get_sound().play("powerup_pickup");
    }
    else
    {
        get_sound().play("food_pickup");
    }
}

void AudioSystem::recieve_ghost_state(const EvGhostStateChanged& state) const
{
    if (state.new_state == EAIState::Dead)
    {
        get_sound().play("ghost_die");
    }
}

void AudioSystem::recieve_pacman_life(const EvPacLifeChanged& life) const
{
    /* Play 'ghost die xD' when pacman dies */
    if (life.delta < 0)
    {
        get_sound().play("ghost_die");
    }

    /* Play game over if we are entirely dead */
    if (life.new_life == 0)
    {
        get_sound().play("game_over");
    }
}

}  // namespace pac
