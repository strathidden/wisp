#include "Renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>

Renderer::Renderer(GLFWwindow* window, int width, int height)
    : m_window(window), m_width(width), m_height(height)
{
    auto computeResult = Shader::createCompute("shaders/raymarch.comp");
    if (!computeResult)
    {
        std::cerr << "Compute shader error: " << computeResult.error() << "\n";
        throw std::runtime_error("Failed to create compute shader");
    }
    m_computeShader = std::move(*computeResult);
    
    auto screenResult = Shader::createVertexFragment("shaders/screen.vert", "shaders/screen.frag");
    if (!screenResult)
    {
        std::cerr << "Screen shader error: " << screenResult.error() << "\n";
        throw std::runtime_error("Failed to create screen shader");
    }
    m_screenShader = std::move(*screenResult);

    createOutputTexture();
    createScreenQuad();
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_screenVAO);
    glDeleteBuffers(1, &m_screenVBO);
    glDeleteTextures(1, &m_outputTexture);
}

void Renderer::resize(int width, int height)
{
    m_width = width;
    m_height = height;

    glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::render(const Camera& camera)
{
    m_computeShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(m_width)/m_height, 0.1f, 100.0f);

    m_computeShader.setMat4("view", view);
    m_computeShader.setMat4("proj", proj);
    m_computeShader.setVec3("cameraPos", camera.getPosition());

    if (m_currentFractal)
    {
        m_currentFractal->updateUniforms(m_computeShader);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    glBindImageTexture(0, m_outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute((m_width + 15) / 16, (m_height + 15) / 16, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_screenShader.use();
    glBindVertexArray(m_screenVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    m_screenShader.setInt("screenTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool Renderer::isMandelbulbActive() const
{
    return m_currentFractal && m_currentFractal->getName() == "Mandelbulb";
}

bool Renderer::isJuliaActive() const
{
    return m_currentFractal && m_currentFractal->getName() == "Julia";
}


std::string_view Renderer::getCurrentFractalName() const
{
    return m_currentFractal ? m_currentFractal->getName() : "None";
}

const FractalParams& Renderer::getCurrentFractalParams() const
{
    static FractalParams defaultParams;
    return m_currentFractal ? m_currentFractal->getParams() : defaultParams;
}

void Renderer::setCurrentFractalParams(const FractalParams& params)
{
    if (m_currentFractal)
    {
        m_currentFractal->setParams(params);
    }
}

void Renderer::createScreenQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Renderer::createOutputTexture() {
    glGenTextures(1, &m_outputTexture);
    glBindTexture(GL_TEXTURE_2D, m_outputTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
