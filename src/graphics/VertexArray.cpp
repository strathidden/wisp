#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    if (m_id)
    {
        glDeleteVertexArrays(1, &m_id);
    }
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_id(other.m_id),
    m_buffers(std::move(other.m_buffers))
{
    other.m_id = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other)
    {
        if (m_id)
        {
            glDeleteVertexArrays(1, &m_id);
        }
        m_id = other.m_id;
        m_buffers = std::move(other.m_buffers);
        other.m_id = 0;
    }
    return *this;
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
{
    bind();
    buffer->bind();

    const auto& layout = buffer->getLayout();
    const auto& elements = layout.getElements();
    unsigned int offset = 0;

    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), reinterpret_cast<const void*>(static_cast<uintptr_t>(offset)));
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }

    m_buffers.push_back(buffer);
}
