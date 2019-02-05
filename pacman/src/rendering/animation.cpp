#include "animation.h"

#include <gfx.h>
#include <cglutil.h>
#include <glad/glad.h>

namespace pac
{
namespace render
{
Animation load_animation(const char* fp, const SpriteSheet& sheet_data)
{
    /* #TODO : Make sure filepath resolves */
    auto pixels = cgl::load_texture(fp);

    /* Create OpenGL Resources */
    GLuint texture = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);

    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Allocate storage and then put texture data into their respective layers */
    glTextureStorage3D(texture, 1, GL_RGBA8, sheet_data.size.x, sheet_data.size.y, sheet_data.num_frames);
    for (unsigned i = 0; i < sheet_data.num_frames; ++i)
    {
        glTextureSubImage3D(texture, 0, 0, 0, i, sheet_data.size.x, sheet_data.size.y, sheet_data.num_frames,
                            GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    Animation out = {};
    out.sprite_sheet = sheet_data;
    out.fps = 24u;
    out.texture = texture;

    return out;
}

}  // namespace render
}  // namespace pac
