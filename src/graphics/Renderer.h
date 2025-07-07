#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    void initialize();
    void render(const Shader& shader);

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};
