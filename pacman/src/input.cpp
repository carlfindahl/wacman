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
        auto* state = static_cast<pac::State*>(glfwGetWindowUserPointer(window));
        state->try_invoke_key(key);
    }

    /* Send some input to ImGui */
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

}  // namespace input
}  // namespace pac
