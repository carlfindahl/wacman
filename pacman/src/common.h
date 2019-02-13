#pragma once

#include <cstdint>

#include <glm/vec2.hpp>

namespace pac
{
namespace detail
{
/*!
 * \brief The custom_ivec2_hash struct is a quick hash that puts x in upper 32 bits and y in lower. Used for mapping dirs to
 * textures
 */
struct custom_ivec2_hash
{
    std::size_t operator()(const glm::ivec2& vec) const noexcept
    {
        const uint64_t hash = (static_cast<uint64_t>(vec.x) << 32u) | vec.y;
        return hash;
    }
};
}  // namespace detail
}  // namespace pac
