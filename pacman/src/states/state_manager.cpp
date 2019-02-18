#include "state_manager.h"

namespace pac
{
bool StateManager::empty() const { return m_statestack.empty(); }

void StateManager::clear() { m_pending_commands.emplace_back(nullptr, ECommandType::Clear); }

void pac::StateManager::pop() { m_pending_commands.emplace_back(nullptr, ECommandType::Pop); }

void StateManager::update(float dt)
{
    /* Process Commands */
    for (auto& command : m_pending_commands)
    {
        switch (command.command_type)
        {
        /* Enter and push state */
        case ECommandType::Push:
            command.new_state->on_enter();
            m_statestack.emplace_back(std::move(command.new_state));
            break;
        /* Exit and pop a state */
        case ECommandType::Pop:
            m_statestack.back()->on_exit();
            m_statestack.pop_back();
            break;
        /* Clear the state stack */
        case ECommandType::Clear:
            for (auto it = m_statestack.rbegin(); it != m_statestack.rend(); ++it)
            {
                (*it)->on_exit();
                m_statestack.pop_back();
            }
            break;
        default: break;
        }
    }

    m_pending_commands.clear();

    /* Update states from top to bottom */
    for (auto it = m_statestack.rbegin(); it != m_statestack.rend(); ++it)
    {
        if (!(*it)->update(dt))
        {
            break;
        }
    }
}

void StateManager::draw()
{
    /* Draw from top to bottom */
    for (auto it = m_statestack.rbegin(); it != m_statestack.rend(); ++it)
    {
        if (!(*it)->draw())
        {
            break;
        }
    }
}

State* StateManager::get_active_state() const { return m_statestack.empty() ? nullptr : m_statestack.back().get(); }
}  // namespace pac
