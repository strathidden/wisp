#include "Application.h"
#include "backends/imgui_impl_glfw.h"
#include <iostream>

Application::Application()
    : m_camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0))
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, "Wisp", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    m_renderer = std::make_unique<Renderer>();
    m_uiManager = std::make_unique<UIManager>(m_window, m_renderer->getFractalParams());
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->m_width = width;
        app->m_height = height;
        app->m_renderer->resize(width, height);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (!app->m_uiManager->wantCaptureMouse()) {
            app->m_camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app->m_uiManager)
        {
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (!app->m_uiManager->wantCaptureMouse()) {
            app->m_camera.processMouseScroll(static_cast<float>(yoffset));
        }
    });
}


Application::~Application()
{
    glfwTerminate();
}

void Application::run()
{
    while (m_running && !glfwWindowShouldClose(m_window))
    {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - m_lastFrameTime);
        m_lastFrameTime = currentTime;

        processInput();
        update();
        render();
        m_uiManager->render(deltaTime);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Application::processInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        m_running = false;
    }

    if (!m_uiManager->wantCaptureKeyboard())
    {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) m_camera.processKeyboard(FORWARD);
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) m_camera.processKeyboard(BACKWARD);
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) m_camera.processKeyboard(LEFT);
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) m_camera.processKeyboard(RIGHT);
    }
}

void Application::update()
{
    m_renderer->updateFractalParams(m_uiManager->getFractalParams());
}

void Application::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    m_renderer->render(m_camera, m_width, m_height);
    glDisable(GL_DEPTH_TEST);
}
