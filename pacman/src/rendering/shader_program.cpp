#include "shader_program.h"

#include <cglutil.h>
#include <glad/glad.h>

namespace pac
{
ShaderProgram::ShaderProgram(const std::vector<cgl::ShaderStage> shader_stages)
{
    m_name = cgl::make_shader_program(shader_stages);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : m_name(other.m_name) { other.m_name = 0u; }

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    /* Delete our old resource before stealing other's */
    glDeleteProgram(m_name);
    m_name = other.m_name;
    other.m_name = 0u;

    return *this;
}

ShaderProgram::~ShaderProgram() noexcept { glDeleteProgram(m_name); }

ShaderProgram::operator unsigned() const { return m_name; }

void ShaderProgram::use() const { glUseProgram(m_name); }

}  // namespace pac
