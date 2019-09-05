#include "behaviour_tree.h"

namespace pac
{
EBehaviourStatus behaviour::tick()
{
    if (m_status != EBehaviourStatus::KeepGoing)
    {
        on_initialize();
    }
    m_status = update();
    if (m_status != EBehaviourStatus::KeepGoing)
    {
        on_terminate();
    }
    return m_status;
}
}  // namespace pac
