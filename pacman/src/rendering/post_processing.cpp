#include "post_processing.h"
#include "config.h"

#include <gfx.h>
#include <cglutil.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace pac
{
PostProcessor::PostProcessor()
    : m_shader({{GL_VERTEX_SHADER, "res/shaders/post.vert"}, {GL_FRAGMENT_SHADER, "res/shaders/post.frag"}})
{
    /* Create framebuffer render texture */
    glCreateTextures(GL_TEXTURE_2D, 1, &m_framebuffer.texture);
    glTextureStorage2D(m_framebuffer.texture, 1, GL_RGBA8, SCREEN_W, SCREEN_H);
    glTextureParameteri(m_framebuffer.texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_framebuffer.texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_framebuffer.texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(m_framebuffer.texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    /* Create renderbuffer */
    glCreateRenderbuffers(1, &m_framebuffer.renderbuffer);
    glNamedRenderbufferStorage(m_framebuffer.renderbuffer, GL_DEPTH24_STENCIL8, SCREEN_W, SCREEN_H);

    /* Create framebuffer */
    glCreateFramebuffers(1, &m_framebuffer.framebuffer);
    glNamedFramebufferTexture(m_framebuffer.framebuffer, GL_COLOR_ATTACHMENT0, m_framebuffer.texture, 0);
    glNamedFramebufferRenderbuffer(m_framebuffer.framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_framebuffer.renderbuffer);

    GFX_ASSERT(glCheckNamedFramebufferStatus(m_framebuffer.framebuffer, GL_DRAW_FRAMEBUFFER), "Framebuffer not complete!");

    /* Set up uniform once since it never changes */
    glProgramUniform1i(m_shader, 0, 0);
}

PostProcessor::~PostProcessor()
{
    glDeleteFramebuffers(1, &m_framebuffer.framebuffer);
    glDeleteRenderbuffers(1, &m_framebuffer.renderbuffer);
    glDeleteTextures(1, &m_framebuffer.texture);
}

void PostProcessor::capture()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer.framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::process()
{
    /* Reset framebuffer state */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);

    m_shader.use();
    glProgramUniform1f(m_shader, 1, glfwGetTime());
    glProgramUniform1i(m_shader, 0, 0);
    glBindTextureUnit(0, m_framebuffer.texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
}  // namespace pac
