#pragma once

#include "states/state.h"
#include "entity/events.h"
#include "level.h"
#include "entity/rendering_system.h"
#include "entity/animation_system.h"
#include "ui.h"

#include <array>
#include <cstdint>
#include <vector>

namespace pac
{
class EditorState : public State
{
private:
    enum class EMode
    {
        TilePlacement,
        EntityPlacement
    };

    /* Level we are editing */
    Level m_level{};

    /* All entities currently active */
    std::vector<std::pair<std::string, glm::ivec2>> m_entities{};

    /* Active Systems */
    std::vector<std::unique_ptr<System>> m_systems{};

    /* Entities availalbe to spawn */
    std::vector<std::string> m_entity_prototypes{};

    /* Level Name */
    std::array<char, 64> m_level_name{};

    /* Current Level Size */
    glm::ivec2 m_size = {28, 36};

    /* Current Cursor Tile */
    glm::ivec2 m_hovered_tile = {0, 0};

    /* Current editor mode */
    EMode m_editor_mode = EMode::TilePlacement;

    /* Current tile tex */
    unsigned m_current_tex = 0u;

    /* Current entity to place */
    std::string m_current_entity{};

    /* ID of Entity that is about to be spawned */
    unsigned m_entity_about_to_spawn = 0u;

    /* Overlay */
    TextureID m_overlay = {};

    /* Tileset selector UI */
    ui::TilesetSelector m_tileselect_ui{};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    /*!
     * \brief recieve_key accepts input actions and updates the editor state based on that
     * \param input is the event data
     */
    void recieve_key(const EvInput& input);

    /*!
     * \brief recieve_mouse accepts mouse input and updates the currently selected editor tile
     * \param input is the event data
     */
    void recieve_mouse(const EvMouseMove& input);

    void set_selection(unsigned selection);

private:
    /*!
     * \brief draw_ui draws the editor UI
     * \param dt is the delta time
     */
    void draw_ui(float dt);

    /*!
     * \brief load_get_entities is called after loading a level to store the current loaded entities in the entity map
     */
    void load_get_entities();

    /*!
     * \brief load_entity_prototypes searches the entity folder for available Entity prototypes so we can choose between them
     */
    void load_entity_prototypes();
};
}  // namespace pac
