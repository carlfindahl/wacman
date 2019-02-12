/*!
 * \file uniform_buffer_object.h contains a simple abstraction around a uniform buffer object.
 */

#pragma once

#include <vector>
#include <cstring>
#include <type_traits>

#include <gfx.h>
#include <glad/glad.h>

namespace pac
{
/*!
 * \class The UniformBuffer class is a relatively thin wrapper around a Uniform Buffer Object.
 */
template<typename DataType>
class UniformBuffer
{
private:
    /* Data Type */
    DataType m_data = {};

    /* OpenGL Name */
    unsigned m_name = 0u;

public:
    using value_type = DataType;

    UniformBuffer() { create(); }

    explicit UniformBuffer(const DataType& data) : m_data(data) { create(); }

    UniformBuffer(const UniformBuffer& other) : m_data(other.m_data) { create(); }

    UniformBuffer& operator=(const UniformBuffer& other)
    {
        if (this == &other)
        {
            return *this;
        }

        /* Destroy existing and then copy */
        glDeleteBuffers(1, &m_name);
        m_data = other.m_data;

        /* Create buffer from copied data */
        create();

        return *this;
    }

    UniformBuffer(UniformBuffer&& other) noexcept : m_data(std::move(other.m_data)), m_name(other.m_name) { other.m_name = 0u; }

    UniformBuffer& operator=(UniformBuffer&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        /* Destroy resource before stealing */
        glDeleteBuffers(1, &m_name);
        m_data = std::move(other.m_data);
        m_name = other.m_name;
        other.m_name = 0u;

        return *this;
    }

    ~UniformBuffer() noexcept { glDeleteBuffers(1, &m_name); }

    /*!
     * \brief update updates the data in the buffer by constructing it's data type from the arguments. For a POD data type,
     * simply forward each argument to each member in the data type, otherwise make sure arguments match exactly 1 ctor of
     * the DataType.
     * \param data is the data required to construct or create the data type
     */
    template<typename... Args>
    void update(Args&&... data)
    {
        m_data = DataType{std::forward<Args>(data)...};
        write_to_gl();
    }

    /*!
     * \brief update updates the data in the buffer from the provided parameter
     * \param data is the new data
     */
    void update(const DataType& data)
    {
        m_data = data;
        write_to_gl();
    }
    void update(DataType&& data)
    {
        m_data = std::move(data);
        write_to_gl();
    }

    /*!
     * \brief bind bind the UBO to a given OpenGL UBO binding point
     * \param binding is the binding number to bind the buffer to
     */
    void bind(unsigned binding)
    {
        GFX_ASSERT(m_name != 0, "Attempt to bind an invalid UBO!");
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_name);
    }

    void unbind(unsigned binding) { glBindBufferBase(GL_UNIFORM_BUFFER, binding, 0u); }

private:
    /*!
     * \brief create creates the initial buffer contents based on m_data
     */
    void create()
    {
        glCreateBuffers(1, &m_name);
        glNamedBufferStorage(m_name, sizeof(DataType), &m_data, GL_MAP_WRITE_BIT);
    }

    /*!
     * \brief write_to_gl writes the CPU contents of the Uniform Buffer to the GPU
     */
    void write_to_gl()
    {
        void* mapped = glMapNamedBufferRange(m_name, 0, sizeof(DataType), GL_MAP_WRITE_BIT);
        if (mapped)
        {
            memcpy(mapped, &m_data, sizeof(DataType));
            glUnmapNamedBuffer(m_name);
            glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
        }
        else
        {
            GFX_WARN("Failed to map UBO #%u", m_name);
        }
    }
};
}  // namespace pac
