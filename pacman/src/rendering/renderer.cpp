#include "renderer.h"

#include <array>
#include <numeric>

#include <gfx.h>
#include <cstring>
#include <cglutil.h>
#include <glad/glad.h>

namespace pac
{
Renderer::Renderer(unsigned max_sprites)
    : m_vao(std::vector<VertexArray::Attribute>{{{0u, 0u, 2, GL_FLOAT, offsetof(Vertex, pos), 0u},
                                                 {1u, 0u, 2, GL_FLOAT, offsetof(Vertex, uv), 0u},
                                                 {2u, 1u, 2, GL_FLOAT, offsetof(InstanceVertex, pos), 1u},
                                                 {3u, 1u, 2, GL_FLOAT, offsetof(InstanceVertex, size), 1u},
                                                 {4u, 1u, 3, GL_FLOAT, offsetof(InstanceVertex, col), 1u},
                                                 {5u, 1u, 1, GL_UNSIGNED_INT, offsetof(InstanceVertex, texture_id), 1u}}})
{
    init(max_sprites);
}

void Renderer::init(unsigned max_sprites)
{
    /* Enable required OpenGL State (Texture alpha blending etc) */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Create Shader Program */
    prog = std::make_unique<ShaderProgram>(
        std::vector<cgl::ShaderStage>{{GL_VERTEX_SHADER, "res/sprite.vert"}, {GL_FRAGMENT_SHADER, "res/sprite.frag"}});

    /* Use program and set sampler values to texture bind points right away, this state is stored in the program so we never
     * need to update this ever again since the texture bind points will be fixed. */
    prog->use();
    std::vector<int> tmp(16u);  // # TODO : Don't use 16 magically
    std::iota(tmp.begin(), tmp.end(), 0u);
    glUniform1iv(0, tmp.size(), tmp.data());

    /* Check maximum amount of texture units and hopefully it's enough :D */
    int max_tex_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units);
    if (max_tex_units < 32)
    {
        GFX_INFO("Application supports only %d/16 texture units.", max_tex_units);
    }
    else
    {
        GFX_INFO("Application supports more than the recommended 16 (%d) texture units! Nice.", max_tex_units);
    }

    /* Init sprite buffer */
    glCreateBuffers(1, &m_sprite_buffer);

    std::array<GLuint, 6> sprite_indices = {0, 1, 2, 2, 3, 0};

    /* Quad is centered, so origin is at {0, 0}, will be scaled in instance data to reach proper size */
    std::array<Vertex, 4> sprite_quad = {Vertex{{-0.5f, -0.5f}, {0.f, 0.f}}, Vertex{{0.5f, -0.5f}, {1.f, 0.f}},
                                         Vertex{{0.5f, 0.5f}, {1.f, 1.f}}, Vertex{{-0.5f, 0.5f}, {0.f, 1.f}}};

    /* Combine vertex and index data into one array and store everything in a single buffer (for locality) */
    std::array<GLubyte, cgl::size_bytes(sprite_indices) + cgl::size_bytes(sprite_quad)> data = {};
    memcpy(data.data(), sprite_indices.data(), cgl::size_bytes(sprite_indices));
    memcpy(data.data() + cgl::size_bytes(sprite_indices), sprite_quad.data(), cgl::size_bytes(sprite_quad));

    /* Combined Data for Initial Contents, not mappable, so Introspection will fail on this VAO */
    glNamedBufferStorage(m_sprite_buffer, cgl::size_bytes(sprite_indices) + cgl::size_bytes(sprite_quad), data.data(), 0u);

    /* Create and allocate space for max_sprites number of sprites */
    glCreateBuffers(1, &m_instance_buffer);
    glNamedBufferStorage(m_instance_buffer, max_sprites * sizeof(InstanceVertex), nullptr, GL_MAP_WRITE_BIT);

    /* Pre-Bind IBO and VBOs for the VAO */
    glVertexArrayElementBuffer(m_vao, m_sprite_buffer);
    glVertexArrayVertexBuffer(m_vao, 0u, m_sprite_buffer, cgl::size_bytes(sprite_indices), sizeof(Vertex));
    glVertexArrayVertexBuffer(m_vao, 1u, m_instance_buffer, 0u, sizeof(InstanceVertex));
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
    Gfx::IntrospectShader("Sprite Shader", *prog);

    /* Write data to GPU */
    void* data = glMapNamedBufferRange(m_instance_buffer, 0, cgl::size_bytes(m_instance_data), GL_MAP_WRITE_BIT);
    if (data)
    {
        memcpy(data, m_instance_data.data(), cgl::size_bytes(m_instance_data));
        glUnmapNamedBuffer(m_instance_buffer);
        glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    }

    /* Prepare state (only needs to bind VAO since it knows about it's resources already. Also bind all textures to their
     *  respective texture units (from 0 an onward. */
    prog->use();
    glBindVertexArray(m_vao);
    glBindTextures(0, m_textures.size(), m_textures.data());
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(m_instance_data.size()));
    m_instance_data.clear();
}

TextureID Renderer::load_texture(std::string_view relative_fp)
{
    auto tex_data = cgl::load_texture(relative_fp.data());

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_id);
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Texture */
    glTextureStorage3D(tex_id, 1, GL_RGBA8, tex_data.width, tex_data.height, 1);
    glTextureSubImage3D(tex_id, 0, 0, 0, 0, tex_data.width, tex_data.height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                        tex_data.pixels.data());

    /* Returns index into texture array */
    m_textures.push_back(tex_id);
    return {0u, 1u, 0u, static_cast<uint8_t>(m_textures.size() - 1)};
}

TextureID Renderer::load_animation_texture(std::string_view relative_fp, int xoffset, int yoffset, int w, int h, int cols,
                                           int count)
{
    auto tex_data = cgl::load_texture_partitioned(relative_fp.data(), xoffset, yoffset, w, h, cols, count);

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_id);
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Texture */
    glTextureStorage3D(tex_id, 1, GL_RGBA8, tex_data[0].width, tex_data[0].height, tex_data.size());
    for (int i = 0; i < static_cast<int>(tex_data.size()); ++i)
    {
        glTextureSubImage3D(tex_id, 0, 0, 0, i, tex_data[i].width, tex_data[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                            tex_data[i].pixels.data());
    }

    /* Return ID */
    m_textures.push_back(tex_id);
    return {0u, static_cast<uint8_t>(tex_data.size()), 0u, static_cast<uint8_t>(m_textures.size() - 1)};
}

Renderer& get_renderer()
{
    static Renderer r{2048};
    return r;
}

}  // namespace pac
