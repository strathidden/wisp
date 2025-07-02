#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, size_t size)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    if (m_id)
    {
        glDeleteBuffers(1, &m_id);
    }
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_id(other.m_id),
    m_layout(other.m_layout)
{
    other.m_id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
    if (this != &other)
    {
        if (m_id)
        {
            glDeleteBuffers(1, &m_id);
        }
        m_id = other.m_id;
        m_layout = other.m_layout;
        other.m_id = 0;
    }
    return *this;
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<>
void VertexBufferLayout::push<float>(unsigned int count)
{
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::push<unsigned int>(unsigned int count)
{
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::push<unsigned char>(unsigned int count)
{
    m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_FALSE});
    m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
}
