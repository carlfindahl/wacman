#pragma once

#include <string>
#include <chrono>

#include <glm/vec2.hpp>

namespace pac
{
namespace render
{
struct SpriteSheet
{
    /* Top left pixel coordinate to start loading from */
    glm::ivec2 top_left = {};

    /* Size of each entry in the sprite sheet */
    glm::ivec2 size = {};

    /* Number of Columns and Rows to load respectively */
    glm::ivec2 col_row_count = {};

    /* Number of frames, in case we have 8 used frames in a 3x3 grid */
    uint32_t num_frames = 0u;
};

struct Animation
{
    /* The sprite sheet data used by this animation */
    SpriteSheet sprite_sheet = {};

    /* The playback FPS */
    unsigned fps = 0u;

    /* The texture is an OpenGL Name that refers to the ID of the texture */
    unsigned texture = 0u;

    /* The current frame of the animation */
    unsigned current_frame = 0u;
};

Animation load_animation(const char* fp, const SpriteSheet& sheet_data);

}  // namespace render

}  // namespace pac
