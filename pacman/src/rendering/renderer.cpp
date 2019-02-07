#include "renderer.h"

#include <array>

#include <cstring>
#include <cglutil.h>
#include <glad/glad.h>

namespace pac
{
Renderer::Renderer(unsigned max_sprites)
{
    /* Init sprite buffer */
    glCreateBuffers(1, &m_sprite_buffer);

    std::array<Vertex, 4> sprite_quads = {Vertex{{-0.5f, -0.5f}, {0.f, 0.f}}, Vertex{{0.5f, -0.5f}, {1.f, 0.f}},
                                          Vertex{{0.5f, 0.5f}, {1.f, 1.f}}, Vertex{{-0.5f, 0.5f}, {0.f, 1.f}}};
    std::array<GLuint, 6> sprite_indices = {0, 1, 2, 2, 3, 0};

    /* Combined Data for Initial Contents */
    std::array<GLubyte, cgl::size_bytes(sprite_quads) + cgl::size_bytes(sprite_indices)> data = {};
    memcpy(data.data(), sprite_quads.data(), cgl::size_bytes(sprite_quads));
    memcpy(data.data() + cgl::size_bytes(sprite_quads), sprite_indices.data(), cgl::size_bytes(sprite_indices));
    glNamedBufferStorage(m_sprite_buffer, cgl::size_bytes(sprite_quads) + cgl::size_bytes(sprite_indices), data.data(), 0u);
}

Renderer::~Renderer()
{
    glDeleteBuffers(1, &m_sprite_buffer);
    glDeleteTextures(m_textures.size(), m_textures.data());
}

void Renderer::draw(const Renderer::InstanceVertex &data, std::size_t texture_id)
{

}

std::size_t Renderer::load_texture(const char* relative_fp)
{
    auto tex_data = cgl::load_texture(relative_fp);

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex_id);
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Image */
    glTextureStorage2D(tex_id, 1, GL_RGBA8, tex_data.width, tex_data.height);
    glTextureSubImage2D(tex_id, 0, 0, 0, tex_data.width, tex_data.height, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.pixels.data());

    /* Returns index into texture array */
    m_textures.push_back(tex_id);
    return m_textures.size() - 1u;
}

std::size_t Renderer::load_animation_texture(const char* relative_fp)
{
    /* TODO : IMPLEMENT */
    return 0u;
}

Renderer& get_renderer()
{
    static Renderer r{2048};
    return r;
}

}  // namespace pac
