#include "post_processing.h"
#include "config.h"

#include <gfx.h>
#include <cglutil.h>
#include <glad/glad.h>

namespace pac
{
PostProcessor::PostProcessor() : m_shader({{GL_VERTEX_SHADER, "res/post.vert"}, {GL_FRAGMENT_SHADER, "res/post.frag"}})
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

void PostProcessor::capture() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer.framebuffer); }

void PostProcessor::process()
{
    /* Reset framebuffer state */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);

    glUseProgram(m_shader);
    glBindTextureUnit(0, m_framebuffer.texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBlitNamedFramebuffer(m_framebuffer.framebuffer, GL_FRAMEBUFFER_DEFAULT, 0, 0, 0, 0, SCREEN_W, SCREEN_H, SCREEN_W, SCREEN_H,
                           GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
}  // namespace pac
