#pragma once

#include <rendering/vertex_array_object.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

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

public:
    GameState();

    ~GameState() noexcept override;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
