#pragma once
#include "Camera.h"
#include "Fractal.h"
#include "Shader.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void render(const Camera& camera, int width, int height) const;
    void updateFractalParams(const FractalParams& params);
    FractalParams& getFractalParams() { return m_fractalParams; }

private:
    void createQuad();

    Shader m_shader;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    FractalParams m_fractalParams;
};
