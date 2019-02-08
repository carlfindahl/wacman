#include "state_manager.h"

namespace pac
{
void pac::StateManager::push(std::unique_ptr<pac::State> new_state) { m_pending_pushes.emplace_back(std::move(new_state)); }

void pac::StateManager::pop() { ++m_pending_pop_count; }

void StateManager::update(float dt)
{
    for (uint8_t i = 0u; i < m_pending_pop_count; ++i)
    {
        m_statestack.back()->on_exit();
        m_statestack.pop_back();
    }

    for (auto& push_op : m_pending_pushes)
    {
        m_statestack.emplace_back(std::move(push_op));
        m_statestack.back()->on_enter();
    }
    m_pending_pushes.clear();

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
