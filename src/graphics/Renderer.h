#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <expected>
#include <string>
#include <memory>
#include <future>
#include "Shader.h"
#include "Framebuffer.h"
#include "VertexArray.h"

class Renderer
{
public:
    struct Config
    {
        int width = 1600;
        int height = 900;
        int msaaSamples = 4;
        bool vsync = true;
    };

    Renderer(GLFWwindow* window, const Config& config);
    ~Renderer();

    std::expected<void, std::string> initialize();
    void resize(int width, int height);

    void beginFrame();
    void endFrame();

    void bindFractalFramebuffer();
    void applyBloom();
    void applyPostProcessing(float exposure, float gamma);
    void renderFullscreenQuad();

    Shader& getFractalShader() { return *m_fractalShader; }
    Shader& getPostProcessingShader() { return *m_postProcessShader; }
    Shader& getBloomShader() { return *m_bloomShader; }

    std::expected<void, std::string> reloadShaders();
    std::future<std::expected<void, std::string>> reloadShadersAsync();

private:
    GLFWwindow* m_window;
    Config m_config;

    std::unique_ptr<Shader> m_fractalShader;
    std::unique_ptr<Shader> m_postProcessShader;
    std::unique_ptr<Shader> m_bloomShader;
    std::unique_ptr<Framebuffer> m_mainFBO;
    std::unique_ptr<Framebuffer> m_bloomFBO;
    std::unique_ptr<VertexArray> m_fullscreenQuad;

    std::expected<std::unique_ptr<Shader>, std::string> createShader(const std::string& vertPath, const std::string& fragPath);
};
