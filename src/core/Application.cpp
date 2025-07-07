#include "Application.h"
#include "../fractals/Mandelbulb.h"
#include "../fractals/MengerSponge.h"
#include "../fractals/IFS.h"
#include <iostream>

Application::Application()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_window = glfwCreateWindow(m_width, m_height, "Fractal Renderer", nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->m_camera.handleMouseMove(x, y);
    });
    
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->m_camera.handleScroll(y);
    });

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    m_shader = Shader("shaders/screen.vert", "shaders/raymarch.frag");

    m_renderer.initialize();
    m_ui.initialize(m_window);

    m_fractalRegistry.registerFractal<Mandelbulb>("Mandelbulb");
    m_fractalRegistry.registerFractal<MengerSponge>("Menger Sponge");
    m_fractalRegistry.registerFractal<IFS>("IFS");

    m_currentFractal = m_fractalRegistry.createFractal("Mandelbulb");
    if (!m_currentFractal)
    {
        std::cerr << "Failed to create initial fractal" << std::endl;
    }
}

Application::~Application()
{
    m_ui.shutdown();
    glfwTerminate();
}

void Application::handleInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window, true);
    }
    
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            double dx, dy;
            glfwGetCursorPos(m_window, &dx, &dy);
            static double lastX = dx, lastY = dy;
            m_camera.pan(dx - lastX, dy - lastY);
            lastX = dx;
            lastY = dy;
        }
        else
        {
            double dx, dy;
            glfwGetCursorPos(m_window, &dx, &dy);
            static double lastX = dx, lastY = dy;
            m_camera.orbit(dx - lastX, dy - lastY);
            lastX = dx;
            lastY = dy;
        }
    }
}

void Application::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        handleInput();
        
        m_ui.beginFrame();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        m_camera.update();
        
        m_shader.use();
        m_shader.setMat4("view", m_camera.getViewMatrix());
        m_shader.setMat4("projection", m_camera.getProjectionMatrix(m_aspect));
        m_shader.setVec3("cameraPosition", m_camera.getPosition());
        
        if (m_currentFractal)
        {
            m_currentFractal->setUniforms(m_shader);
        }
        
        std::vector<glm::vec2> quad =
        {
            {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f},
            {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f}
        };
        m_renderer.render(m_shader, quad);
        
        m_ui.draw(*this, *m_currentFractal);
        m_ui.endFrame();
        
        glfwSwapBuffers(m_window);
    }
}
