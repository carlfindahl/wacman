#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pac
{
/*!
 * \brief The Renderer class is a specialized "renderer" designed to render this pacman game efficiently. It
 * uses a single, large buffer starting with vertex and index data for the one instanced quad. Then the rest
 * of the buffer is per-instance data for all the sprites in the game.
 * \note This class should be instanced once, and act as a Singleton ish
 */
class Renderer
{
private:
    /* Buffer that contains sprite data */
    unsigned m_sprite_buffer = 0u;

    /* Buffer that contains per-instance data */
    unsigned m_instance_buffer = 0u;

    /* Vector of all currently available textures */
    std::vector<unsigned> m_textures = {};

    /*!
     * \brief The Vertex struct is the vertex layout for the sprite quad
     */
    struct Vertex
    {
        glm::vec2 pos = {};
        glm::vec2 uv = {};
    };

    /*!
     * \brief The InstanceVertex struct is the vertex layout of per-instance sprite instantiations
     */
    struct InstanceVertex
    {
        glm::vec2 pos = {};
        glm::vec3 col = {};
    };

public:
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();

    void draw(const InstanceVertex& data, std::size_t texture_id);

    /*!
     * \brief load_texture loads the texture at the given relative file path
     * \param relative_fp is the relative file path
     * \return a handle to the new texture, you do not own this, so please do not delete it or otherwise be careless with it
     */
    std::size_t load_texture(const char* relative_fp);

    /*!
     * \brief load_animation_texture loads a texture with an animated sprite in it that can later be used with an animation
     * \param relative_fp is the relative file path
     * \return a handle to the new texture, you do not own this, so please do not delete it or otherwise be careless with it
     */
    std::size_t load_animation_texture(const char* relative_fp);

private:
    /* Private because we want the singleton function to be the only one able to create a Renderer */
    explicit Renderer(unsigned max_sprites = 2048u);
    friend Renderer& get_renderer();
};

/*!
 * \brief get_renderer returns a reference to the active renderer Singleton class
 * \note singleton is accessible through this function so we can control the lifetime and creation, unlike a global which
 * is created statically and can mess with the GL Context or otherwise act weirdly.
 * \return The Renderer
 */
Renderer& get_renderer();
}  // namespace pac
