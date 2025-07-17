#include "Renderer.h"

Renderer::Renderer()
{
    m_shader.load("shaders/screen.vert", "shaders/fractal.frag");
    createQuad();
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Renderer::render(const Camera& camera, int width, int height) const
{
    m_shader.use();

    m_shader.setVec3("u_cameraPosition", camera.getPosition());
    m_shader.setMat4("u_viewMatrix", camera.getViewMatrix());
    m_shader.setMat4("u_projectionMatrix", camera.getProjectionMatrix(width, height));
    m_shader.setVec2("u_resolution", glm::vec2(width, height));
    
    m_shader.setInt("u_maxIterations", m_fractalParams.maxIterations);
    m_shader.setFloat("u_power", m_fractalParams.power);
    m_shader.setFloat("u_bailout", m_fractalParams.bailout);
    m_shader.setFloat("u_scale", m_fractalParams.scale);
    m_shader.setVec3("u_offset", m_fractalParams.offset);
    
    m_shader.setFloat("u_minDistance", m_fractalParams.minDistance);
    m_shader.setFloat("u_maxDistance", m_fractalParams.maxDistance);
    m_shader.setVec3("u_color1", m_fractalParams.color1);
    m_shader.setVec3("u_color2", m_fractalParams.color2);
    m_shader.setFloat("u_ambient", m_fractalParams.ambient);
    m_shader.setFloat("u_diffuse", m_fractalParams.diffuse);
    m_shader.setFloat("u_specular", m_fractalParams.specular);
    m_shader.setFloat("u_shininess", m_fractalParams.shininess);
    m_shader.setInt("u_maxSteps", m_fractalParams.maxSteps);
    m_shader.setInt("u_samples", m_fractalParams.samples);
    
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::updateFractalParams(const FractalParams& params)
{
    m_fractalParams = params;
}

void Renderer::createQuad()
{
    const float quad[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
