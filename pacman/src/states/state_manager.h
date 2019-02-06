#pragma once

#include "state.h"

#include <memory>
#include <vector>

namespace pac
{
class StateManager
{
private:
    /* Game State Stack */
    std::vector<std::unique_ptr<State>> m_statestack = {};

    std::vector<std::unique_ptr<State>> m_pending_pushes = {};

    uint8_t m_pending_pop_count = 0u;

public:
    /*!
     * \brief push push a new state on the state stack
     * \param new_state is the state to push, is unique ptr to hint that we are transferring ownership
     */
    void push(std::unique_ptr<State> new_state);

    /*!
     * \brief pop a state from the stack
     */
    void pop();

    /*!
     * \brief update update all the stack from top to bottom until we hit a blocking state
     * \param dt delta time
     */
    void update(float dt);

    /*!
     * \brief draw draws the active states
     */
    void draw();

    /*!
     * \brief get_active_state
     * \return nullptr if no active state, otherwise a non-owning pointer to the active state. Don't store it
     * after a pop, as it will be invalid.
     */
    State* get_active_state() const;
};
}  // namespace pac
