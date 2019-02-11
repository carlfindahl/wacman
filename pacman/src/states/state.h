#pragma once

#include <input.h>

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
public:
    State() = default;

    State(const State&) = delete;

    State(State&& other) noexcept = default;

    State& operator=(const State&) = delete;

    State& operator=(State&& other) noexcept = default;

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
    virtual bool update(float dt) = 0;

    /*!
     * \brief draw is responsible for drawing
     * \return true if you want to allow states below this one to draw also
     */
    virtual bool draw() = 0;
};
}  // namespace pac
