#pragma once

#include "state.h"

#include <memory>
#include <vector>

namespace pac
{
class State;

class StateManager
{
private:
    /*!
     * \brief The ECommandType enum represents a command that can be executed by the state manager.
     */
    enum class ECommandType : uint16_t
    {
        Nothing,
        Push,
        Pop
    };

    struct Command
    {
        /* Used when Command is Push */
        std::unique_ptr<State> new_state = nullptr;

        /* The command to execute */
        ECommandType command_type = ECommandType::Nothing;

        /* So we can emplace back */
        Command(std::unique_ptr<State> state, ECommandType type) : new_state(std::move(state)), command_type(type) {}
    };

    /* Game State Stack */
    std::vector<std::unique_ptr<State>> m_statestack = {};

    /* Commands waiting */
    std::vector<Command> m_pending_commands = {};

public:
    /*!
     * \brief push push a new state on the state stack
     * \param new_state is the state to push, is unique ptr to hint that we are transferring ownership
     */
    template<typename State_>
    void push()
    {
        m_pending_commands.emplace_back(std::make_unique<State_>(*this), ECommandType::Push);
    }

    /*!
     * \brief empty checks if the state stack is empty
     * \return true if the state stack is empty
     */
    bool empty() const;

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
