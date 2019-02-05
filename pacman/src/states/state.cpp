#include "state.h"

namespace pac
{
State::State(State&& other) noexcept : m_keybindings(std::move(other.m_keybindings)) {}

State& State::operator=(State&& other) noexcept
{
    /* No point moving from the same object */
    if (&other == this)
    {
        return *this;
    }

    /* No destructor work, so just steal data */
    m_keybindings = std::move(other.m_keybindings);
    return *this;
}

void State::try_invoke_key(int glfw_key)
{
    if (auto it = m_keybindings.find(glfw_key); it != m_keybindings.end())
    {
        it->second();
    }
}

void State::bind_key(int glfw_key, std::function<void()> action)
{
    m_keybindings.emplace(glfw_key, std::move(action));
}

void State::unbind_key(int glfw_key) { m_keybindings.erase(glfw_key); }

}  // namespace pac
