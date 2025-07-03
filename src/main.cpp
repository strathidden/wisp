#include "graphics/Renderer.h"
#include "fractal/Mandelbulb.h"
#include "fractal/Julia.h"
#include "ui/UIManager.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <expected>
#include <string>
#include <nlohmann/json.hpp>

struct AppState {
    bool showUI = true;
    bool captureMouse = false;
    bool openSaveModal = false;
    bool openLoadModal = false;
};

std::expected<GLFWwindow*, std::string> createWindow(int width, int height)
{
    if (!glfwInit()) return std::unexpected("Failed to initialize GLFW");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    GLFWwindow* window = glfwCreateWindow(width, height, "Wisp", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return std::unexpected("Failed to create GLFW window");
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) renderer->resize(width, height);
}

int main()
{
    constexpr int WIDTH = 1920;
    constexpr int HEIGHT = 1080;
    
    auto windowResult = createWindow(WIDTH, HEIGHT);
    if (!windowResult)
    {
        std::cerr << windowResult.error() << "\n";
        return -1;
    }
    GLFWwindow* window = *windowResult;
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
    
    Renderer renderer(window, WIDTH, HEIGHT);
    UIManager uiManager(window);
    AppState appState;
    
    Camera camera;
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            auto* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            if (!camera) return;
            
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            
            if (action == GLFW_PRESS)
            {
                if (button == GLFW_MOUSE_BUTTON_LEFT)
                    camera->start(Camera::Action::Rotate, {static_cast<float>(x), static_cast<float>(y)});
                else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                    camera->start(Camera::Action::Pan, {static_cast<float>(x), static_cast<float>(y)});
            } 
            else if (action == GLFW_RELEASE)
            {
                camera->end();
            }
        }
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            auto* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            if (camera)
            {
                camera->zoom(static_cast<float>(yoffset) * 0.1f);
            }
        }
    });
    
    Mandelbulb mandelbulb;
    Julia julia;
    renderer.setFractal(mandelbulb);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        float fps = 1.0f / deltaTime;
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
            appState.showUI = !appState.showUI;
        
        if (camera.isActive())
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            camera.process({static_cast<float>(x), static_cast<float>(y)});
        }
        
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.render(camera);
        
        if (appState.showUI)
        {
            uiManager.beginFrame();
            
            ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(400, 550), ImGuiCond_FirstUseEver);
            
            if (ImGui::Begin("Wisp", &appState.showUI, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::Button("Mandelbulb", ImVec2(120, 30))) 
                    renderer.setFractal(mandelbulb);

                ImGui::SameLine();

                if (ImGui::Button("Julia", ImVec2(120, 30))) 
                    renderer.setFractal(julia);
                
                ImGui::SameLine();

                if (ImGui::Button("Save Config", ImVec2(120, 30)))
                    appState.openSaveModal = true;

                ImGui::SameLine();

                if (ImGui::Button("Load Config", ImVec2(120, 30)))
                    appState.openLoadModal = true;
                
                ImGui::Separator();
                
                if (renderer.isMandelbulbActive())
                    uiManager.renderFractalControls(mandelbulb, "Mandelbulb Parameters");
                else
                    uiManager.renderFractalControls(julia, "Julia Parameters");
                
                ImGui::Separator();
                
                ImGui::Text("Camera Position: %.2f, %.2f, %.2f", 
                    camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
                ImGui::Text("Target: %.2f, %.2f, %.2f", 
                    camera.getTarget().x, camera.getTarget().y, camera.getTarget().z);
                ImGui::Text("Distance: %.2f", camera.getDistance());
            }
            ImGui::End();
            
            uiManager.renderSaveLoadModal(
                appState.openSaveModal, 
                appState.openLoadModal,
                std::string(renderer.getCurrentFractalName()),
                renderer.getCurrentFractalParams()
            );
            
            uiManager.renderPerformance(fps, deltaTime * 1000);
            uiManager.endFrame();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
