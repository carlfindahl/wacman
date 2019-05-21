#include "input.h"
#include "entity/events.h"

#include <gfx.h>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

namespace pac
{
extern entt::dispatcher g_event_queue;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* Inoke inputs if it's a key press */
    if (action == GLFW_PRESS && !ImGui::GetIO().WantTextInput)
    {
        get_input().invoke(key);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    static glm::dvec2 s_previous_positions{};

    /* Compute mouse delta */
    glm::vec2 delta = glm::dvec2{xpos, ypos} - s_previous_positions;

    /* Reset previous callback update values to the new positions */
    s_previous_positions = {xpos, ypos};

    get_input().invoke_axis(delta.x, -delta.y);
    get_input().set_cursor_pos({xpos, ypos});
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        get_input().invoke(button);
    }
}

void InputDomain::bind_key(int key, Action action) { m_bindings[key] = action; }

void InputDomain::bind_live_key(int key, Action action) { m_live_bindings[key] = action; }

void InputDomain::try_invoke(int key)
{
    if (auto it = m_bindings.find(key); it != m_bindings.end())
    {
        g_event_queue.enqueue(EvInput{it->second});
    }
}

void InputDomain::invoke_live_keys(float dt, GLFWwindow* win)
{
    for (auto& m_live_binding : m_live_bindings)
    {
        if (glfwGetKey(win, m_live_binding.first) ||
            (glfwGetMouseButton(win, m_live_binding.first) && !ImGui::GetIO().WantCaptureMouse))
        {
            g_event_queue.enqueue(EvInput{m_live_binding.second});
        }
    }
}

bool InputDomain::blocking() const { return m_blocking; }

void InputManager::push(InputDomain&& state) { m_waiting_commands.emplace_back(std::move(state), ECommandType::Push); }

void InputManager::pop() { m_waiting_commands.emplace_back(ECommandType::Pop); }

void InputManager::invoke(int key)
{
    for (auto it = m_active_domains.rbegin(); it != m_active_domains.rend(); ++it)
    {
        it->try_invoke(key);
        if (it->blocking())
        {
            break;
        }
    }
}

void InputManager::update(float dt, GLFWwindow* win)
{
    /* Process changes in the input stack */
    for (auto& cmd : m_waiting_commands)
    {
        switch (cmd.command_type)
        {
        case ECommandType::Push: m_active_domains.push_back(std::move(cmd.new_state)); break;
        case ECommandType::Pop: m_active_domains.pop_back(); break;
        case ECommandType::Clear: m_active_domains.clear(); break;
        default: break;
        }
    }

    m_waiting_commands.clear();

    /* Invoke keys */
    for (auto it = m_active_domains.rbegin(); it != m_active_domains.rend(); ++it)
    {
        it->invoke_live_keys(dt, win);
        if (it->blocking())
        {
            break;
        }
    }
}

void InputManager::invoke_axis(float x, float y) { g_event_queue.enqueue(EvMouseMove{{x, y}, m_mouse_pos}); }

void InputManager::set_cursor_pos(const glm::dvec2& new_pos) { m_mouse_pos = new_pos; }

const glm::dvec2& InputManager::get_cursor_position() const { return m_mouse_pos; }

InputManager& get_input()
{
    static InputManager im{};
    return im;
}
}  // namespace pac
