/*!
 * \file input.h contains implementations of input handling and callbacks. It also contains the InputManager
 * which is a higher level construct that wrap the callbacks in a simpler to use interface.
 */

#pragma once

#include <vector>
#include <functional>
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

/*!
 * \brief The InputState class contains a set of key bindings.
 */
class InputState
{
private:
    /* Map of keybindings */
    std::unordered_map<int, std::function<void(void)>> m_bindings = {};

    /* Whether this state is blocking */
    bool m_blocking = false;

public:
    InputState() = default;

    /*!
     * \brief InputState
     * \param blocking if this state should block other states
     */
    InputState(bool blocking) : m_blocking(blocking){};

    /*!
     * \brief set_binding sets an action to be performed when the given key is pressed
     * \param key is the key to bind
     * \param action is the action to perform (function)
     */
    void set_binding(int key, std::function<void(void)> action);

    /*!
     * \brief remove_binding removes the binding of key
     * \param key the key to remove
     */
    void remove_binding(int key);

    /*!
     * \brief try_invoke attempts to invoke the given key binding
     * \param key is the key to try to invoke
     */
    void try_invoke(int key);

    /*!
     * \brief blocking
     * \return true if state is blocking
     */
    bool blocking() const;
};

/*!
 * \brief The InputManager class is a singleton that gives the program access to bind various input
 */
class InputManager
{
private:
    /* Stack of input_states */
    std::vector<InputState> m_input_states = {};

public:
    /*!
     * \brief push an input state to the stack
     * \param state is the input state to push
     */
    void push(const InputState& state);
    void push(InputState&& state);

    /*!
     * \brief pop an input state from the stack
     */
    void pop();

    /*!
     * \brief invoke the input manager with the given key.
     */
    void invoke(int key);

private:
    InputManager() = default;

    friend InputManager& get_input();
};

/*!
 * \brief get_input returns access to the global input manager
 * \return the input manager
 */
InputManager& get_input();
}  // namespace input
}  // namespace pac
