#pragma once
#include <glad/glad.h>
#include <vector>

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() = default;

    template<typename T>
    void push(unsigned int count);

    const std::vector<VertexBufferElement>& getElements() const { return m_elements; }

    unsigned int getStride() const { return m_stride; }

private:
    std::vector<VertexBufferElement> m_elements;
    unsigned int m_stride = 0;
};

class VertexBuffer
{
public:
    VertexBuffer(const void* data, size_t size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void bind() const;
    static void unbind();

    const VertexBufferLayout& getLayout() const { return m_layout; }
    void setLayout(const VertexBufferLayout& layout) { m_layout = layout; }

private:
    GLuint m_id = 0;
    VertexBufferLayout m_layout;
};
