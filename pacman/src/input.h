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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/*!
 * \brief The EActions enum describes all possible actions supported by the game.
 */
enum class EAction
{
    NOP,           // Does nothing
    TURN_LEFT,     // Turn pacman left
    TURN_RIGHT,    // Turn pacman right
    TURN_UP,       // Turn pacman up
    TURN_DOWN,     // Turn pacman down
    MENU_UP,       // Navigate the menu upwards
    MENU_DOWN,     // Navigate the menu downwards
    MENU_CONFIRM,  // Confirm / select in the menu
    BACK_CANCEL    // Go back in the menus OR pause / cancel an active action
};

/*!
 * \brief The InputManager class is responsible for recieving low level inputs from GLFW and translating them
 * into actions that are mapped from the keys which the game then listens to and processes on a higher level.
 */
class InputManager
{
private:
    /* Maps keys to actions */
    std::unordered_map<int, EAction> m_keymap = {};

public:
};
}  // namespace input
}  // namespace pac
