#include "renderer.h"

#include <array>

#include <gfx.h>
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
    memcpy(data.data(), sprite_indices.data(), cgl::size_bytes(sprite_indices));
    memcpy(data.data() + cgl::size_bytes(sprite_indices), sprite_quads.data(), cgl::size_bytes(sprite_quads));
    glNamedBufferStorage(m_sprite_buffer, cgl::size_bytes(sprite_quads) + cgl::size_bytes(sprite_indices), data.data(),
                         GL_MAP_READ_BIT);

    /* Create and allocate space for max_sprites number of sprites */
    glCreateBuffers(1, &m_instance_buffer);
    glNamedBufferStorage(m_instance_buffer, max_sprites * sizeof(InstanceVertex), nullptr, GL_MAP_WRITE_BIT);

    /* Pre-Bind IBO and VBOs for the VAO */
    glVertexArrayElementBuffer(m_vao, m_sprite_buffer);
}

Renderer::Renderer()
    : m_vao(std::vector<VertexArray::Attribute>{{{0u, 0u, 2, GL_FLOAT, offsetof(Vertex, pos), 0u},
                                                 {1u, 0u, 2, GL_FLOAT, offsetof(Vertex, uv), 0u},
                                                 {2u, 1u, 2, GL_FLOAT, offsetof(InstanceVertex, pos), 1u},
                                                 {3u, 1u, 2, GL_FLOAT, offsetof(InstanceVertex, size), 1u},
                                                 {4u, 1u, 3, GL_FLOAT, offsetof(InstanceVertex, col), 1u},
                                                 {5u, 1u, 1, GL_UNSIGNED_INT, offsetof(InstanceVertex, texture_id), 1u}}})
{
}

Renderer::~Renderer()
{
    glDeleteBuffers(1, &m_sprite_buffer);
    glDeleteBuffers(1, &m_instance_buffer);
    glDeleteTextures(m_textures.size(), m_textures.data());
}

void Renderer::draw(const Renderer::InstanceVertex& data) { m_instance_data.push_back(data); }

void Renderer::submit_work()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glBindVertexArray(m_vao);
    glBindVertexBuffer(0u, m_sprite_buffer, 24u, sizeof(Vertex));
    glBindVertexBuffer(1u, m_instance_buffer, 0u, sizeof(InstanceVertex));
    Gfx::IntrospectVertexArray("Sprite VAO", m_vao);

    /* Write data to GPU */
    void* data = glMapNamedBufferRange(m_instance_buffer, 0, cgl::size_bytes(m_instance_data), GL_MAP_WRITE_BIT);
    if (data)
    {
        auto* iv_data = static_cast<InstanceVertex*>(data);
        for (auto i = 0u; i < m_instance_data.size(); ++i)
        {
            iv_data[i] = m_instance_data[i];
        }

        glUnmapNamedBuffer(m_instance_buffer);
        glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    }

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, m_instance_data.size());
    m_instance_data.clear();
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
