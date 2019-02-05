#pragma once

#include <utility>
#include <functional>
#include <unordered_map>

namespace pac
{
class State
{
private:
    std::unordered_map<int, std::function<void()>> m_keybindings = {};

public:
    State() = default;

    State(const State&) = delete;

    State(State&& other) noexcept;

    State& operator=(const State&) = delete;

    State& operator=(State&& other) noexcept;

    virtual ~State() noexcept = default;

    virtual void on_enter() = 0;

    virtual void on_exit() = 0;

    virtual void update(float dt) = 0;

    void try_invoke_key(int glfw_key);

    void bind_key(int glfw_key, std::function<void()> action);

    void unbind_key(int glfw_key);
};
}  // namespace pac
