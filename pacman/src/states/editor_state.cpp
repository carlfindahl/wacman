#include "editor_state.h"
#include "input/input.h"
#include "config.h"
#include "rendering/renderer.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <entt/signal/dispatcher.hpp>
#include <entt/meta/factory.hpp>

namespace pac
{
/* The game event queue */
extern entt::dispatcher g_event_queue;

void EditorState::on_enter()
{
    InputDomain editor_domain(true);
    editor_domain.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    editor_domain.bind_live_key(GLFW_MOUSE_BUTTON_LEFT, ACTION_PLACE);
    editor_domain.bind_live_key(GLFW_MOUSE_BUTTON_RIGHT, ACTION_UNDO);
    editor_domain.bind_key(GLFW_MOUSE_BUTTON_MIDDLE, ACTION_CLONE);
    editor_domain.bind_key(GLFW_KEY_T, ACTION_TOGGLE_DEBUG);
    editor_domain.bind_key(GLFW_KEY_E, ACTION_NEXT_TILE);
    editor_domain.bind_key(GLFW_KEY_Q, ACTION_PREV_TILE);
    get_input().push(std::move(editor_domain));

    /* Add Systems */
    m_systems.emplace_back(std::make_unique<AnimationSystem>());
    m_systems.emplace_back(std::make_unique<RenderingSystem>());

    /* Overlay for reference */
    m_overlay = get_renderer().load_texture("res/ingame_overlay.png");

    m_level.resize(m_size);

    g_event_queue.sink<EvInput>().connect<&EditorState::recieve_key>(this);
    g_event_queue.sink<EvMouseMove>().connect<&EditorState::recieve_mouse>(this);
}

void EditorState::on_exit()
{
    g_event_queue.sink<EvInput>().disconnect<&EditorState::recieve_key>(this);
    g_event_queue.sink<EvMouseMove>().disconnect<&EditorState::recieve_mouse>(this);

    get_input().pop();
    m_context.registry->reset();
}

bool EditorState::update(float dt)
{
    draw_ui(dt);
    for (auto& system : m_systems)
    {
        system->update(dt, *m_context.registry);
    }
    return false;
}

bool EditorState::draw()
{
    m_level.draw();

    get_renderer().draw({{HALF_TILE + glm::vec2(m_hovered_tile) * TILE_SIZE<float>},
                         glm::vec2(TILE_SIZE<float>),
                         glm::vec3(.5f, 1.f, 0.5f),
                         get_renderer().get_tileset_texture(m_tileset_tex)});

    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_overlay});
    return false;
}

void EditorState::recieve_key(const EvInput& input)
{
    auto& tile = m_level.get_tile(m_hovered_tile);

    switch (input.action)
    {
    case ACTION_PLACE:
        tile.type = static_cast<Level::ETileType>(m_tileset_tex);
        tile.texture = get_renderer().get_tileset_texture(m_tileset_tex);
        break;
    case ACTION_UNDO:
        tile.type = Level::ETileType::Blank;
        tile.texture = {};
        break;
    case ACTION_CLONE: m_tileset_tex = tile.texture.frame_number; break;
    case ACTION_NEXT_TILE: ++m_tileset_tex; break;
    case ACTION_PREV_TILE: --m_tileset_tex; break;
    default: break;
    }
}

void EditorState::recieve_mouse(const EvMouseMove& input) { m_hovered_tile = input.position / TILE_SIZE<float>; }

void EditorState::draw_ui(float dt)
{
    ImGui::Begin("Editor");

    /* Saving and Loading */
    ImGui::InputText("Level Name", m_level_name.data(), cgl::size_bytes(m_level_name));
    if (ImGui::Button("Load"))
    {
        m_level.load(*m_context.lua, *m_context.registry, m_level_name.data());
    }
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        m_level.save(*m_context.lua, *m_context.registry, m_level_name.data(), m_entities);
    }

    /* Level Properties */
    if (ImGui::DragInt2("Size", glm::value_ptr(m_size), 1.f, 5, 36))
    {
        m_level.resize(m_size);
    }

    ImGui::End();
}

}  // namespace pac
