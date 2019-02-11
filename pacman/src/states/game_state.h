#pragma once

#include <level.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace pac
{
namespace detail
{
struct MatrixData
{
    glm::mat4 proj_matrix = glm::mat4(1.f);
    glm::mat4 view_matrix = glm::mat4(1.f);
};
}  // namespace detail

class GameState : public State
{
private:
    /* Matrix UBO */
    UniformBuffer<detail::MatrixData> m_ubo = {};

    /* The level / world */
    Level m_level = {};

public:
    GameState();

    ~GameState() noexcept override;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
