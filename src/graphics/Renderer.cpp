#include "Renderer.h"
#include "../core/Logger.h"
#include <glad/glad.h>
#include <fstream>

Renderer::Renderer(GLFWwindow* window, const Config& config)
    : m_window(window), m_config(config) {}

Renderer::~Renderer()
{
    if (m_window)
    {
        glfwMakeContextCurrent(m_window);
    }
}

std::expected<void, std::string> Renderer::initialize()
{
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return std::unexpected("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(m_config.vsync ? 1 : 0);

    auto fractalShader = createShader("shaders/screen.vert", "shaders/fractal.frag");
    if (!fractalShader)
    {
        return std::unexpected(fractalShader.error());
    }
    m_fractalShader = std::move(*fractalShader);

    auto postShader = createShader("shaders/screen.vert", "shaders/postprocess.frag");
    if (!postShader)
    {
        return std::unexpected(postShader.error());
    }
    m_postProcessShader = std::move(*postShader);

    auto bloomShader = createShader("shaders/screen.vert", "shaders/bloom.frag");
    if (!bloomShader)
    {
        return std::unexpected(bloomShader.error());
    }
    m_bloomShader = std::move(*bloomShader);

    m_mainFBO = std::make_unique<Framebuffer>();
    if (!m_mainFBO->create(m_config.width, m_config.height, true, m_config.msaaSamples))
    {
        return std::unexpected("Failed to create main FBO");
    }

    m_bloomFBO = std::make_unique<Framebuffer>();
    if (!m_bloomFBO->create(m_config.width, m_config.height, false))
    {
        return std::unexpected("Failed to create bloom FBO");
    }

    float quadVertices[] =
    {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    m_fullscreenQuad = std::make_unique<VertexArray>();
    auto vertexBuffer = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    vertexBuffer->setLayout(layout);
    m_fullscreenQuad->addVertexBuffer(vertexBuffer);

    return {};
}

void Renderer::resize(int width, int height)
{
    m_config.width = width;
    m_config.height = height;
    glViewport(0, 0, width, height);
    m_mainFBO->resize(width, height);
    m_bloomFBO->resize(width, height);
}

void Renderer::beginFrame()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame()
{
    glfwSwapBuffers(m_window);
}

void Renderer::bindFractalFramebuffer()
{
    m_mainFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::applyBloom()
{
    m_bloomFBO->bind();
    m_bloomShader->use();
    m_bloomShader->setInt("image", 0);
    m_mainFBO->bindColorTexture(0);
    renderFullscreenQuad();
    Framebuffer::unbindAll();
}

void Renderer::applyPostProcessing(float exposure, float gamma)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_postProcessShader->use();
    m_postProcessShader->setInt("scene", 0);
    m_postProcessShader->setInt("bloomBlur", 1);
    m_postProcessShader->setFloat("exposure", exposure);
    m_postProcessShader->setFloat("gamma", gamma);

    m_mainFBO->bindColorTexture(0);
    m_bloomFBO->bindColorTexture(1);

    renderFullscreenQuad();
}

void Renderer::renderFullscreenQuad()
{
    m_fullscreenQuad->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    VertexArray::unbind();
}

std::expected<std::unique_ptr<Shader>, std::string> Renderer::createShader(const std::string& vertPath, const std::string& fragPath)
{
    auto shader = std::make_unique<Shader>();
    if (!shader->load(vertPath, fragPath))
    {
        return std::unexpected(shader->getLastError());
    }
    return shader;
}

std::expected<void, std::string> Renderer::reloadShaders()
{
    auto fractalShader = createShader("shaders/screen.vert", "shaders/fractal.frag");
    if (!fractalShader)
    {
        return std::unexpected(fractalShader.error());
    }
    m_fractalShader = std::move(*fractalShader);

    auto postShader = createShader("shaders/screen.vert", "shaders/postprocess.frag");
    if (!postShader)
    {
        return std::unexpected(postShader.error());
    }
    m_postProcessShader = std::move(*postShader);

    auto bloomShader = createShader("shaders/screen.vert", "shaders/bloom.frag");
    if (!bloomShader)
    {
        return std::unexpected(bloomShader.error());
    }
    m_bloomShader = std::move(*bloomShader);

    return {};
}

std::future<std::expected<void, std::string>> Renderer::reloadShadersAsync()
{
    return std::async(std::launch::async, [this] { return reloadShaders(); });
}
