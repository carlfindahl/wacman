#pragma once

#include "shader_program.h"

namespace pac
{
class PostProcessor
{
public:
    /*!
     * \brief The Filter enum describes the various filters that can be applied in post proccessing
     */
    enum class Filter
    {
        BlackAndWhite,
        VerticalLines,
        SphericalDistort,
        ChromaticAbberation
    };

private:
    /*!
     * \brief The FrameBuffer struct represents all GL objects required to have a PP framebuffer
     */
    struct FrameBuffer {
        unsigned renderbuffer = 0u;
        unsigned framebuffer = 0u;
        unsigned texture = 0u;
    };

    /* Post processing framebuffer */
    FrameBuffer m_framebuffer = {};

    /* The post processing shader */
    ShaderProgram m_shader;

public:
    PostProcessor();

    ~PostProcessor();

    /*!
     * \brief capture begins capturing image information from any draw calls that are drawn. Call this before you draw anything
     * that you want to apply processing to
     */
    void capture();

    /*!
     * \brief process stops capturing and does the post processing work on all previously captured draw calls. Call this after you
     * have drawn what you want to apply effects to
     */
    void process();
};

}  // namespace pac
