/*!
 * \file shader_program.h contains a wrapper around an OpenGL Shader Program
 */

#pragma once

#include <vector>

/* Forward Declare */
namespace cgl
{
struct ShaderStage;
}

namespace pac
{
/*!
 * \brief The ShaderProgram class wraps an OpenGL shader program loosely. It does not contain functions for using uniforms
 * or any of the like, and provides a transparent handle to the underlying name via the get() member function. It will delete
 * the resource on destruction, and you can simplyfy using it by calling use. It is implicitly convertible to Unsigned, so
 * you can pass this to any other OpenGL function with no issues.
 */
class ShaderProgram
{
private:
    /* OpenGL Name */
    unsigned m_name = {};

public:
    /*!
     * \param shader_stages is a vector of ShaderStages, which contain a shader type, and a file path. This ctor can not
     * construct from in memory shader source code.
     */
    explicit ShaderProgram(const std::vector<cgl::ShaderStage> shader_stages);
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;
    ~ShaderProgram() noexcept;

    operator unsigned() const;

    void use() const;
};
}  // namespace pac
