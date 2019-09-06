#include "behaviour_tree.h"

#include <algorithm>

namespace pac
{
EBehaviourStatus behaviour::tick()
{
    if (m_status != EBehaviourStatus::Running)
    {
        on_initialize();
    }
    m_status = update();
    if (m_status != EBehaviourStatus::Running)
    {
        on_terminate();
    }
    return m_status;
}

behaviour* composite::add_behaviour(std::unique_ptr<behaviour> behaviour)
{
    m_children.emplace_back(std::move(behaviour));
    return m_children.back().get();
}

bool composite::remove_behaviour(behaviour* behaviour)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), [&behaviour](const auto& b) { return b.get() == behaviour; });
    if (it != m_children.end())
    {
        m_children.erase(it);
        return true;
    }

    return false;
}

void composite::clear() { m_children.clear(); }

}  // namespace pac
