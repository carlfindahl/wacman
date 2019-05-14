#include "renderer.h"
#include "config.h"

#include <array>
#include <numeric>

#include <gfx.h>
#include <sstream>
#include <cstring>
#include <cglutil.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>

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
    m_ubo.update(glm::ortho<float>(0.f, SCREEN_W, SCREEN_H, 0.f), glm::mat4(1.f));
}

void Renderer::init(unsigned max_sprites)
{
    /* Enable required OpenGL State (texture alpha blending) */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Create Shader Program */
    prog = std::make_unique<ShaderProgram>(
        std::vector<cgl::ShaderStage>{{GL_VERTEX_SHADER, "res/sprite.vert"}, {GL_FRAGMENT_SHADER, "res/sprite.frag"}});

    /* Use program and set sampler values to texture bind points right away, this state is stored in the program so we never
     * need to update this ever again since the texture bind points will be fixed. */
    prog->use();
    std::vector<int> tmp(MAX_TEXTURES);
    std::iota(tmp.begin(), tmp.end(), 0u);
    glUniform1iv(0, tmp.size(), tmp.data());

    /* Init sprite buffer */
    glCreateBuffers(1, &m_sprite_buffer);

    /* Quad is centered, so origin is at {0, 0}, will be scaled in instance data to reach proper size */
    std::array<Vertex, 4> sprite_quad = {Vertex{{-0.5f, -0.5f}, {0.f, 0.f}}, Vertex{{0.5f, -0.5f}, {1.f, 0.f}},
                                         Vertex{{0.5f, 0.5f}, {1.f, 1.f}}, Vertex{{-0.5f, 0.5f}, {0.f, 1.f}}};
    std::array<GLuint, 6> sprite_indices = {0, 1, 2, 2, 3, 0};

    /* Combine vertex and index data into one array and store everything in a single buffer (for locality) */
    std::array<GLubyte, cgl::size_bytes(sprite_indices) + cgl::size_bytes(sprite_quad)> data = {};
    memcpy(data.data(), sprite_indices.data(), cgl::size_bytes(sprite_indices));
    memcpy(data.data() + cgl::size_bytes(sprite_indices), sprite_quad.data(), cgl::size_bytes(sprite_quad));

    /* Combined Data for Initial Contents, not mappable, so Introspection will fail on this VAO */
    glNamedBufferStorage(m_sprite_buffer, cgl::size_bytes(sprite_indices) + cgl::size_bytes(sprite_quad), data.data(), 0u);

    /* Create and allocate space for max_sprites number of sprites */
    glCreateBuffers(1, &m_instance_buffer);
    glNamedBufferStorage(m_instance_buffer, max_sprites * sizeof(InstanceVertex), nullptr,
                         GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

    /* Immediately map the buffer persistently */
    m_mapped_instance_buffer = glMapNamedBufferRange(m_instance_buffer, 0, max_sprites * sizeof(InstanceVertex),
                                                     GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

    /* Pre-Bind IBO and VBOs for the VAO */
    glVertexArrayElementBuffer(m_vao, m_sprite_buffer);
    glVertexArrayVertexBuffer(m_vao, 0u, m_sprite_buffer, cgl::size_bytes(sprite_indices), sizeof(Vertex));
    glVertexArrayVertexBuffer(m_vao, 1u, m_instance_buffer, 0u, sizeof(InstanceVertex));

    /* Load default texture ID 0 -> blank.png */
    load_texture("res/blank.png");
}

Renderer::~Renderer()
{
    /* Unmap the buffer when the renderer is destroyed */
    glUnmapNamedBuffer(m_instance_buffer);
    glDeleteBuffers(1, &m_sprite_buffer);
    glDeleteBuffers(1, &m_instance_buffer);
    glDeleteTextures(m_textures.size(), m_textures.data());
}

void Renderer::draw(const Renderer::InstanceVertex& data) { m_instance_data.push_back(data); }

void Renderer::draw(InstanceVertex&& data) { m_instance_data.emplace_back(data); }

void Renderer::submit_work()
{
    /* Write data to GPU (The buffer is persistently mapped and explicitly flushed after profiling showed that mapping every frame
     * was very expensive) */
    memcpy(m_mapped_instance_buffer, m_instance_data.data(), cgl::size_bytes(m_instance_data));
    glFlushMappedNamedBufferRange(m_instance_buffer, 0, cgl::size_bytes(m_instance_data));

    /* Prepare state (only needs to bind VAO since it knows about it's resources already. Also bind all textures to their
     *  respective texture units (from 0 an onward to N). */
    prog->use();
    m_ubo.bind(0);
    glBindVertexArray(m_vao);
    glBindTextures(0, m_textures.size(), m_textures.data());
    m_post_processor.capture();
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(m_instance_data.size()));
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    m_post_processor.process();
    m_instance_data.clear();
}

TextureID Renderer::load_texture(std::string_view relative_fp)
{
    /* If texture is loaded already, return it */
    if (auto out = check_texture_is_loaded(relative_fp))
    {
        GFX_DEBUG("Skip reload of texture [%s]", relative_fp.data());
        return *out;
    }

    auto tex_data = cgl::load_texture(relative_fp.data());

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_id);
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Texture */
    glTextureStorage3D(tex_id, 1, GL_RGBA8, tex_data.width, tex_data.height, 1);
    glTextureSubImage3D(tex_id, 0, 0, 0, 0, tex_data.width, tex_data.height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                        tex_data.pixels.data());

    /* Return texture ID and add to cache */
    m_textures.push_back(tex_id);
    GFX_DEBUG("You have loaded %u textures now.", m_textures.size());

    TextureID out{0u, 1u, 0u, static_cast<uint8_t>(m_textures.size() - 1)};
    m_loaded_texture_cache.emplace(relative_fp.data(), out);
    return out;
}

TextureID Renderer::get_tileset_texture(unsigned no)
{
    static auto id = load_animation_texture("res/textures/tileset.png", 0, 0, 25, 25, 4, 21);
    id.frame_number = no;
    return id;
}

TextureID Renderer::load_animation_texture(std::string_view relative_fp, int xoffset, int yoffset, int w, int h, int cols,
                                           int count)
{
    /* Check if it is loaded already, by using all parameters as a string, and then hashing on that*/
    std::stringstream hash_str_stream{};
    hash_str_stream << relative_fp << xoffset << yoffset << w << h << cols << count;
    const auto hash_string = hash_str_stream.str();

    if (auto out = check_texture_is_loaded(hash_str_stream.str()))
    {
        GFX_DEBUG("Skip re-load of animation %s, (%d %d)-(%d %d) c%d n%d", relative_fp.data(), xoffset, yoffset, w, h, cols,
                  count);
        return *out;
    }

    /* Load the animation or spritesheet based on parameters */
    auto tex_data = cgl::load_texture_partitioned(relative_fp.data(), xoffset, yoffset, w, h, cols, count);

    GLuint tex_id = 0u;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_id);
    glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Allocate and transfer data to Texture */
    glTextureStorage3D(tex_id, 1, GL_RGBA8, tex_data[0].width, tex_data[0].height, tex_data.size());
    for (int i = 0; i < static_cast<int>(tex_data.size()); ++i)
    {
        glTextureSubImage3D(tex_id, 0, 0, 0, i, tex_data[i].width, tex_data[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                            tex_data[i].pixels.data());
    }

    /* Return ID and add to texture cache */
    m_textures.push_back(tex_id);
    GFX_DEBUG("You have loaded %u textures now.", m_textures.size());

    TextureID out{0u, static_cast<uint8_t>(tex_data.size()), 0u, static_cast<uint8_t>(m_textures.size() - 1)};
    m_loaded_texture_cache.emplace(hash_string, out);
    return out;
}

std::optional<TextureID> Renderer::check_texture_is_loaded(std::string_view fp)
{
    /* Check for existence in hash map and then return if found */
    if (m_loaded_texture_cache.find(fp.data()) != m_loaded_texture_cache.end())
    {
        GFX_DEBUG("Not loading %s, since it is already loaded.", fp.data());
        auto out = m_loaded_texture_cache[fp.data()];
        return out;
    }

    return std::nullopt;
}

Renderer& get_renderer()
{
    static Renderer r{2048};
    return r;
}

}  // namespace pac
