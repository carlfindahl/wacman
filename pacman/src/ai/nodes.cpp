#include "nodes.h"

namespace pac
{
void pac::sequence::on_initialize() { m_current = m_children.begin(); }

EBehaviourStatus sequence::update() { return EBehaviourStatus::Success; }

void sequence::on_terminate() {}

}  // namespace pac
