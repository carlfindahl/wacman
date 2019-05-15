#pragma once

#include "states/state.h"
#include "entity/events.h"
#include "level.h"

#include <array>
#include <cstdint>

namespace pac
{
class EditorState : public State
{
private:
    /* Level we are editing */
    Level m_level{};

    /* Current Level Size */
    glm::ivec2 m_size = {28, 36};

    /* Current Cursor Tile */
    glm::ivec2 m_hovered_tile = {0, 0};

    /* Current tile tex */
    unsigned m_tileset_tex = 0u;

    /* Overlay */
    TextureID m_overlay = {};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void recieve_key(const EvInput& input);

    void recieve_mouse(const EvMouseMove& input);

private:
    /*!
     * \brief draw_ui draws the editor UI
     * \param dt is the delta time
     */
    void draw_ui(float dt);
};
}  // namespace pac
