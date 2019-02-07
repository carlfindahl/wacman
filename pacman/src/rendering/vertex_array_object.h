#pragma once

#include <vector>

namespace pac
{
class VertexArray
{
private:
    unsigned m_name = 0u;

public:
    /*!
     * \brief The Attribute struct contains all data required to define a single Vertex Attribute
     */
    struct Attribute
    {
        /* Attribute location, specified in shader layout qualifier */
        unsigned location;

        /* Buffer binding point to assosciate with attribute location */
        unsigned binding;

        /* Number of components (1-4) for the attribute */
        int components;

        /* Type of component (GL_FLOAT, GL_UNSIGNED, etc.) */
        unsigned type;

        /* Offset from start of Vertex */
        unsigned offset;

        /* If this is non-zero, it the attribute is per_instance, and not per_vertex */
        unsigned instance_divisor = 0u;
    };

    VertexArray();
    VertexArray(const std::vector<Attribute>& attributes);
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;
    ~VertexArray() noexcept;

    operator unsigned();

    void bind();

private:
    void set_attribute(const Attribute& attr);
};
}  // namespace pac
