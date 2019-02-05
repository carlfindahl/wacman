#pragma once

#include <utility>
#include <functional>
#include <unordered_map>

namespace pac
{
/*!
 * \brief The State class describes a specifc game state (IE: Menu, Playnig, Highscore) and contains state
 * and functionality to accomodate this. It is a base class of the various states to allow states be stored
 * in a heterogeneous container and to elegantly call different update methods based on the dynamic type of
 * the state.
 */
class State
{
private:
    /* Keybindings for the state */
    std::unordered_map<int, std::function<void()>> m_keybindings = {};

public:
    State() = default;

    State(const State&) = delete;

    State(State&& other) noexcept;

    State& operator=(const State&) = delete;

    State& operator=(State&& other) noexcept;

    virtual ~State() noexcept = default;

    /*!
     * \brief on_enter is called when the state is entered
     */
    virtual void on_enter() = 0;

    /*!
     * \brief on_exit is called before the state is exited
     */
    virtual void on_exit() = 0;

    /*!
     * \brief update is called every frame
     * \param dt is the delta time
     */
    virtual void update(float dt) = 0;

    /*!
     * \brief try_invoke_key attempts to invoke the action / function assosciated with a given key
     * \param glfw_key is the key to invoke
     */
    void try_invoke_key(int glfw_key);

    /*!
     * \brief bind_key assosciates an action / function with the given key
     * \param glfw_key is the key to bind to the action
     * \param action is the action to be performed
     */
    void bind_key(int glfw_key, std::function<void()> action);

    /*!
     * \brief unbind_key removes the action / function assosciated with the given key
     * \param glfw_key is the key to unbind
     */
    void unbind_key(int glfw_key) noexcept;
};
}  // namespace pac
