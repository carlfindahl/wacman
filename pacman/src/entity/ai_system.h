#pragma once

#include "system.h"
#include "events.h"
#include "components.h"

#include <glm/vec2.hpp>

namespace pac
{
class Level;

/*!
 * \brief The AISystem handles Ghost AI
 */
class AISystem : public System
{
private:
    Level& m_level;

public:
    AISystem(entt::registry& reg, Level& level);

    ~AISystem() noexcept override;

    void update(float dt) override;

    void recieve(const EvEntityMoved& move);

    void recieve_pacmanstate(const EvPacInvulnreableChange& pac);

private:
    /*!
     * \brief get_player_pos gets the position of the player
     * \param reg is the registry
     * \return player position
     */
    glm::ivec2 get_player_pos() const;

    void pathfind(const CPosition& pos, const glm::ivec2& ai_dir, const glm::ivec2& plr_pos, CAI& ai);
};
}  // namespace pac
