#pragma once

#include "behaviour_tree.h"

namespace pac
{
/*!
 * \brief The sequence node executes it's children in sequence
 */
class sequence : public composite
{
protected:
    /** Current behaviour */
    behaviours::iterator m_current = m_children.begin();

protected:
    void on_initialize() override;

    EBehaviourStatus update() override;

    void on_terminate() override;
};
}  // namespace pac
