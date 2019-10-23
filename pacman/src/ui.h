#pragma once

#include "entity/components.h"
#include "rendering/renderer.h"

#include <array>
#include <vector>

#include <entt/signal/sigh.hpp>

namespace pac
{
namespace ui
{
/*!
 * \brief The TilesetSelector creates UI to select tiles from the current tile-set
 */
class TilesetSelector
{
private:
    /* The tileset being edited */
    TextureID m_tileset;

    /* Current selected */
    unsigned m_selected = 0u;

    /* Signal for when a tile is selected */
    entt::sigh<void(unsigned)> select_tile{};

public:
    TilesetSelector();
    TilesetSelector(TextureID texture);

    /*!
     * \brief update draws the UI and updates it
     * \param dt is delta time
     */
    void update(float dt);

    /*!
     * \brief set_selection sets the selection of this tileset selector
     * \param s is the new selection
     */
    void set_selection(unsigned s);

    /* Sink to handle user connections to the signal */
    entt::sink<void(unsigned)> on_select_tile{select_tile};
};

/*!
 * \brief The AnimationEditor let you edit animations and preview them
 */
class AnimationEditor
{
private:
    /*!
     * \brief The AnimData struct holds information about animations
     */
    struct AnimData
    {
        char texture_name[64] = {'\0'};
        int start_x = 0;
        int start_y = 0;
        int width = 0;
        int height = 0;
        int columns = 0;
        int length = 0;
    } m_current_anim{};

    /* Current animation as a texture handle */
    unsigned m_current_tex{};

    /* Number of frames in current tex */
    unsigned m_current_tex_frames{};

    /* Current frame */
    unsigned m_current_frame{};

    /* Frame timer for Animation */
    float m_frame_timer = 0.f;

public:
    AnimationEditor() = default;

    ~AnimationEditor() noexcept;

    /*!
     * \brief update draws the UI and updates it
     * \param dt is delta time
     */
    void update(float dt);
};

/*!
 * \brief The CampaignEditor class is made to let you chain levels together, but is currently not in use
 */
class CampaignEditor
{
private:
    /* Levels part of this campaign (in order) */
    std::vector<std::string> m_levels{};

    /* Levels you can add to this campaign */
    std::vector<std::string> m_available_levels{};

    /* Campaign Name */
    std::array<char, 64> m_name{};

public:
    CampaignEditor() = default;

    /*!
     * \brief update draws the UI
     * \param dt delta time
     */
    void update(float dt);

private:
    /*!
     * \brief save Save campaign to campaign file
     */
    void save();
};

/*!
 * \brief The LevelSelector class allows you to select a level to play
 */
class LevelSelector
{
private:
    /* Levels to select from */
    std::vector<std::string> m_levels{};

    GameContext m_context;

public:
    LevelSelector(GameContext context);
    ;

    /*!
     * \brief update draws the UI
     * \param dt delta time
     */
    void update(float dt);
};
}  // namespace ui
}  // namespace pac
