#include "vertex_array_object.h"

#include <glad/glad.h>

namespace pac
{
VertexArray::VertexArray() { glCreateVertexArrays(1, &m_name); }

VertexArray::VertexArray(const std::vector<VertexArray::Attribute>& attributes)
{
    glCreateVertexArrays(1, &m_name);
    for (const auto& attribute : attributes)
    {
        set_attribute(attribute);
    }
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_name(other.m_name) { other.m_name = 0u; }

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    glDeleteVertexArrays(1, &m_name);
    m_name = other.m_name;
    other.m_name = 0u;

    return *this;
}

VertexArray::~VertexArray() noexcept { glDeleteVertexArrays(1, &m_name); }

void VertexArray::bind() { glBindVertexArray(m_name); }

pac::VertexArray::operator unsigned() { return m_name; }

void VertexArray::set_attribute(const VertexArray::Attribute& attr)
{
    /* If it is an integral attribute, use correct AttribIFormat */
    switch (attr.type)
    {
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_BYTE: glVertexArrayAttribIFormat(m_name, attr.location, attr.components, attr.type, attr.offset); break;
    default: glVertexArrayAttribFormat(m_name, attr.location, attr.components, attr.type, false, attr.offset); break;
    }

    glVertexArrayAttribBinding(m_name, attr.location, attr.binding);
    glEnableVertexArrayAttrib(m_name, attr.location);

    if (attr.instance_divisor > 0)
    {
        glVertexArrayBindingDivisor(m_name, attr.binding, attr.instance_divisor);
    }
}

}  // namespace pac
