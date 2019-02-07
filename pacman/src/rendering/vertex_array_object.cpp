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
    glVertexArrayAttribFormat(m_name, attr.location, attr.components, attr.type, false, attr.offset);
    glVertexArrayAttribBinding(m_name, attr.location, attr.binding);
    glVertexArrayBindingDivisor(m_name, attr.binding, attr.instance_divisor);
    glEnableVertexArrayAttrib(m_name, attr.location);
}

}  // namespace pac
