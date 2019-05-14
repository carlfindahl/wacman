#pragma once

#include "states/state.h"
#include "entity/events.h"
#include "level.h"

#include <array>
#include <cstdint>

namespace pac
{
class EditorState : public State
{
private:
    /* Level we are editing */
    Level m_level{};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void recieve(const EvInput& input);
};
}  // namespace pac
