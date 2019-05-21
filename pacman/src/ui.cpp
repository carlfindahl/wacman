#include "ui.h"
#include "states/state_manager.h"
#include "states/respawn_state.h"
#include "states/game_state.h"

#include <fstream>

#include <cglutil.h>
#include <imgui/imgui.h>

namespace pac
{
namespace ui
{
/* ---- TILESET EDITOR ---- */

TilesetSelector::TilesetSelector() : m_tileset(get_renderer().get_tileset_texture(0u)) {}

TilesetSelector::TilesetSelector(TextureID texture) : m_tileset(texture) {}

void TilesetSelector::update(float dt)
{
    using namespace ImGui;

    auto frame_tex = m_tileset;
    for (auto i = 0u; i < m_tileset.frame_count; ++i)
    {
        frame_tex.frame_number = i;

        /* If we click the tileset btn, then signal that we have changed selection (void* cast is fine since it's 8 bytes) */
        if (ImageButton((void*)get_renderer().get_texture_for_imgui(frame_tex), {25, 25}, {0, 0}, {1, 1}, -1, {0, 0, 0, 0},
                        (m_selected == i ? ImVec4{1.f, 1.f, 1.f, 1.f} : ImVec4{0.5f, 0.5f, 0.5f, 0.5f})))
        {
            m_selected = i;
            on_select_tile.publish(static_cast<unsigned>(i));
        }

        if ((i + 1) % 6 != 0)
        {
            SameLine();
        }
    }
}

void TilesetSelector::set_selection(unsigned s) { m_selected = s; }

/* ---- ANIMATION EDITOR ---- */

AnimationEditor::~AnimationEditor() noexcept { glDeleteTextures(1, &m_current_tex); }

void AnimationEditor::update(float dt)
{
    using namespace ImGui;
    InputText("Texture", m_current_anim.texture_name, 64);
    InputInt("Start X", &m_current_anim.start_x, 1, 5);
    InputInt("Start Y", &m_current_anim.start_y, 1, 5);
    InputInt("Width", &m_current_anim.width, 1, 5);
    InputInt("Height", &m_current_anim.height, 1, 5);
    InputInt("Columns", &m_current_anim.columns, 1, 2);
    InputInt("Length", &m_current_anim.length, 1, 2);

    /* Commit to preview */
    if (Button("Preview"))
    {
        glDeleteTextures(1, &m_current_tex);
        char final_buffer[256];
        sprintf(final_buffer, "res/textures/%s", m_current_anim.texture_name);
        m_current_tex =
            cgl::load_gl_texture_partitioned(final_buffer, m_current_anim.start_x, m_current_anim.start_y, m_current_anim.width,
                                             m_current_anim.height, m_current_anim.columns, m_current_anim.length);
        m_current_tex_frames = m_current_anim.length;
        m_current_frame = 0u;
        m_frame_timer = 0.f;
    }

    SameLine();

    if (Button("Save"))
    {
        /* TODO: Save Animation */
    }

    /* Only preview if there is anything to preview */
    if (m_current_tex != 0u)
    {
        Text("Preview:");
        SameLine();
        Image((void*)(static_cast<uint64_t>(m_current_tex) | (static_cast<uint64_t>(m_current_frame) << 32u)), {25, 25});
    }

    /* Update animation */
    m_frame_timer += dt;
    if (m_frame_timer > 1.f / 24.f && m_current_tex_frames != 0u)
    {
        m_frame_timer = 0.f;
        m_current_frame = (m_current_frame + 1u) % m_current_tex_frames;
    }
}

/* ---- CAMPAIGN EDITOR ---- */

void CampaignEditor::update(float dt)
{
    using namespace ImGui;
    Columns(2, "CampaignCol", false);
    BeginChild("AvailableCampaignLevels", {0, 0}, true);
    for (auto itr = m_available_levels.crbegin(); itr != m_available_levels.crend(); ++itr)
    {
        if (Selectable(itr->c_str()))
        {
            m_levels.push_back(*itr);
        }
    }
    NextColumn();

    /* Show selected levels */
    Text("Campaign Levels");
    BeginChild("SelectedCampaignLevels");
    for (auto i = 0u; i < m_levels.size(); ++i)
    {
        if (Selectable(m_levels[i].c_str()))
        {
            m_levels.erase(m_levels.begin() + i);
        }
    }

    /* Input text */
    InputText("Campaign Name", m_name.data(), m_name.size());
    if (Button("Save"))
    {
        save();
    }
    NextColumn();
    Columns();
}

void CampaignEditor::save() {}

/* ---- LEVEL SELECTOR ---- */

LevelSelector::LevelSelector(GameContext context) : m_context(context)
{
    context.lua->script_file(cgl::native_absolute_path("res/levels.lua"));
    sol::table levels = (*context.lua)["levels"];
    for (const auto& [k, _] : levels)
    {
        m_levels.push_back(k.as<std::string>());
    }
    std::sort(m_levels.begin(), m_levels.end());
}

void LevelSelector::update(float dt)
{
    using namespace ImGui;

    for (const auto& level : m_levels)
    {
        char btn_txt[64];
        sprintf(btn_txt, "%s##BTN", level.c_str());
        if (Button(btn_txt))
        {
            m_context.state_manager->push<GameState>(m_context, level);
            m_context.state_manager->push<RespawnState>(m_context);
        }
    }
}

}  // namespace ui
}  // namespace pac
