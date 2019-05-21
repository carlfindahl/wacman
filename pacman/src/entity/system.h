#pragma once

#include <entt/entity/registry.hpp>

namespace pac
{
/*!
 * \brief The System class represents a separate part of logic that should be updated in the game world. For example a movement
 * system should handle all movement updates.
 */
class System
{
protected:
    /* Registry this system is working against */
    entt::registry& m_reg;

public:
    System(entt::registry& reg) : m_reg(reg){};
    System(const System&) = default;
    System(System&&) = default;
    System& operator=(System&&) = delete;
    System& operator=(const System&) = delete;
    virtual ~System() noexcept = default;

    /*!
     * \brief update is called to update the system
     * \param dt is the delta time
     * \param reg is the current entity registry to work with
     */
    virtual void update(float dt) = 0;
};

}  // namespace pac
