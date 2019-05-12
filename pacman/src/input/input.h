/*!
 * \file input.h contains implementations of input handling and callbacks. It also contains the InputManager
 * which is a higher level construct that contain InputStates.
 */

#pragma once

#include <array>
#include <vector>
#include <functional>

#include <robinhood/robinhood.h>
#include <glm/vec2.hpp>

/* Forward declare to avoid including glfw.h in this file */
struct GLFWwindow;

namespace pac
{
enum Action
{
    ACTION_NONE,

    /* Core Actions */
    ACTION_QUIT,
    ACTION_TOGGLE_DEBUG,

    /* In-Game Actions */
    ACTION_MOVE_NORTH,
    ACTION_MOVE_EAST,
    ACTION_MOVE_SOUTH,
    ACTION_MOVE_WEST,

    ACTION_CONFIRM,
    ACTION_CANCEL,
    ACTION_PAUSE,

    /* Editor Actions */
    ACTION_PLACE,
    ACTION_UNDO,
    ACTION_NEXT_TILE,
    ACTION_PREV_TILE,
    ACTION_NEXT_ENTITY,
    ACTION_PREV_ENTITY
};

/*!
 * \brief key_callback handles key events suchs as presses and releases
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/*!
 * \brief cursor_position_callback handles cursor movement
 */
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

/*!
 * \brief mouse_button_callback handles mouse button clicks
 */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

/*!
 * \brief The InputState class contains a set of key bindings.
 */
class InputDomain
{
private:
    /* Map of keybindings */
    robin_hood::unordered_map<int, Action> m_bindings{};

    /* Map of live keybindings [polling instead of event based] */
    robin_hood::unordered_map<int, Action> m_live_bindings{};

    /* Array of all axis bindings (since there are only 2 axes, we can afford storing it locally */
    std::array<std::vector<Action>, 2> m_axis_bindings = {};

    /* Whether this state is blocking */
    bool m_blocking = false;

public:
    InputDomain() = default;

    /*!
     * \brief The EAxis enum contains the various input axes that the InputState can handle. It is a regular enum instead of a
     * scoped enum so we can implicitly convert it to an int.
     */
    enum Axis
    {
        AXIS_Horizontal,
        AXIS_Vertical
    };

    /*!
     * \brief InputState
     * \param blocking if this state should block other states
     */
    InputDomain(bool blocking) : m_blocking(blocking){};

    /*!
     * \brief bind_key sets an action to be performed when the given key is pressed
     * \param key is the key to bind (GLFW_KEY_xxx)
     * \param action is the action to assosciate
     */
    void bind_key(int key, Action action);

    /*!
     * \brief bind_live_key sets an action to be performed when the given key is pressed
     * \param key is the key to bind (GLFW_KEY_xxx)
     * \param action is the action to perform (function)
     */
    void bind_live_key(int key, Action action);

    /*!
     * \brief bind_mouse_axis binds an axis to a function that takes a float. This float is the value of the axis ranging from -N
     * to N, depending on how much movement is happening in that particular axis. This is made to work with mouse input, and will
     * be the mouse movement delta by default.
     * \param axis is the axis you want to bind an action to \param action is the action
     * you want to bind
     */
    void bind_mouse_axis(Axis axis, Action action);

    /*!
     * \brief try_invoke attempts to invoke the given key binding
     * \param key is the key to try to invoke
     */
    void try_invoke(int key);

    /*!
     * \brief invoke_live_keys attempts to invoke live key bindings
     * \param key key to invoke
     * \param dt is the delta time
     */
    void invoke_live_keys(float dt, GLFWwindow* win);

    /*!
     * \brief update_axes updates each axis in the Input State
     * \param axis_values are the axis deltas since the last axis update
     */
    void update_axes(float x, float y);

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
    std::vector<InputDomain> m_input_states = {};

    /*!
     * \brief The ECommandType enum represents a command that can be executed by the state manager.
     */
    enum class ECommandType : uint16_t
    {
        Nothing,
        Push,
        Pop,
        Clear
    };

    struct Command
    {
        /* Used when Command is Push */
        InputDomain new_state{};

        /* The command to execute */
        ECommandType command_type = ECommandType::Nothing;

        /* So we can emplace back */
        Command(ECommandType type) : command_type(type) {}

        Command(const InputDomain& state, ECommandType type) : new_state(state), command_type(type) {}

        Command(InputDomain&& state, ECommandType type) : new_state(std::move(state)), command_type(type) {}
    };

    /* Commands waiting */
    std::vector<Command> m_waiting_commands = {};

    /* Last recorded mouse position */
    glm::dvec2 m_mouse_pos = {};

public:
    /*!
     * \brief push an input state to the stack
     * \param state is the input state to push
     */
    void push(const InputDomain& state);
    void push(InputDomain&& state);

    /*!
     * \brief pop an input state from the stack
     */
    void pop();

    /*!
     * \brief invoke the input manager with the given key.
     */
    void invoke(int key);

    /*!
     * \brief update the input manager. Has the effect of polling for glfw keys and calling bound keys
     * \param dt is the delta time
     * \param win is the window to poll
     */
    void update(float dt, GLFWwindow* win);

    /*!
     * \brief invoke_axis invokes the input manager with the given axis values
     * \param x is the x axis delta
     * \param y is the y axis delta
     */
    void invoke_axis(float x, float y);

    /*!
     * \brief set_cursor_pos records the current cursor position
     * \param new_pos is the new position
     */
    void set_cursor_pos(const glm::dvec2& new_pos);

    /*!
     * \brief get_cursor_position gets the current cursor position
     */
    const glm::dvec2& get_cursor_position() const;

private:
    /* Private CTOR since we only want one InputManager */
    InputManager() = default;

    friend InputManager& get_input();
};

/*!
 * \brief get_input returns access to the global input manager
 * \return the input manager
 */
InputManager& get_input();
}  // namespace pac
