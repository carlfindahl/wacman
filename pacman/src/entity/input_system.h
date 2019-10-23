#pragma once

#include "system.h"
#include "events.h"

namespace pac
{
/*!
 * \brief The InputSystem handles actions that players want to take
 */
class InputSystem : public System
{
private:
    /* Actions not yet processed */
    std::vector<Action> m_unprocessed_actions{};

public:
    InputSystem(entt::registry& reg);

    ~InputSystem() noexcept override;

    void update(float dt) override;

    /*!
     * \brief recieve
     * \param input
     */
    void recieve(const EvInput& input);
};
}  // namespace pac
