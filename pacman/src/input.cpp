#include "input.h"
#include <states/state.h>

#include <GLFW/glfw3.h>

namespace pac
{
namespace input
{
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        get_input().invoke(key);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

void InputState::set_binding(int key, std::function<void()> action) { m_bindings[key] = std::move(action); }

void InputState::remove_binding(int key) { m_bindings.erase(key); }

void InputState::try_invoke(int key)
{
    if (auto it = m_bindings.find(key); it != m_bindings.end())
    {
        it->second();
    }
}

bool InputState::blocking() const { return m_blocking; }

InputManager& get_input()
{
    static InputManager im{};
    return im;
}

void InputManager::push(const InputState& state) { m_input_states.push_back(state); }

void InputManager::push(InputState&& state) { m_input_states.push_back(std::move(state)); }

void InputManager::pop() { m_input_states.pop_back(); }

void InputManager::invoke(int key)
{
    for (auto it = m_input_states.rbegin(); it != m_input_states.rend(); ++it)
    {
        it->try_invoke(key);
        if (it->blocking())
        {
            break;
        }
    }
}

}  // namespace input
}  // namespace pac
