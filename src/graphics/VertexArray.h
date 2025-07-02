#pragma once
#include <glad/glad.h>
#include <memory>
#include <vector>
#include "VertexBuffer.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind() const;
    static void unbind();

    void addVertexBuffer(std::shared_ptr<VertexBuffer> buffer);

private:
    GLuint m_id = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_buffers;
};
