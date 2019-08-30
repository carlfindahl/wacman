#include "input_system.h"
#include "components.h"

#include <entt/signal/dispatcher.hpp>

namespace pac
{
extern entt::dispatcher g_event_queue;

InputSystem::InputSystem(entt::registry& reg) : System(reg)
{
    g_event_queue.sink<EvInput>().connect<&InputSystem::recieve>(*this);
}

InputSystem::~InputSystem() noexcept { g_event_queue.sink<EvInput>().disconnect<&InputSystem::recieve>(*this); }

void InputSystem::update(float dt)
{
    auto inputs = m_reg.view<CInput>();

    /* For each input component, check if any unprocessed actions match with their accepted input */
    inputs.each([this](entt::entity e, CInput& input) {
        for (auto action : m_unprocessed_actions)
        {
            if (auto found = input.actions.find(action); found != input.actions.end())
            {
                found->second.call(e);
            }
        }
    });

    /* Clear actions as they are now processed */
    m_unprocessed_actions.clear();
}

void InputSystem::recieve(const EvInput& input) { m_unprocessed_actions.push_back(input.action); }

}  // namespace pac
