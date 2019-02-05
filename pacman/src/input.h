/*!
 * \file input.h contains implementations of input handling and callbacks. It also contains the InputManager
 * which is a higher level construct that wrap the callbacks in a simpler to use interface.
 */

#pragma once

#include <unordered_map>

/* Forward declare to avoid including glfw.h in this file */
struct GLFWwindow;

namespace pac
{
namespace input
{
/*!
 * \brief key_callback handles key evens suchs as presses and releases
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/*!
 * \brief cursor_position_callback handles cursor movement (only relevant for ImGui in this project)
 */
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

/*!
 * \brief mouse_button_callback handles mouse button clicks (only relevant for ImGui in this project)
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
}  // namespace input
}  // namespace pac
