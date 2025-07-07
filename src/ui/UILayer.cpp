#include "UILayer.h"
#include "../core/Application.h"
#include "../fractals/FractalRegistry.h"
#include "../utils/JsonHelper.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void UILayer::initialize(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void UILayer::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UILayer::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UILayer::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UILayer::draw(Application& app)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
    drawMainMenu(app);
    drawFractalControls(app.m_currentFractal.get());
}

void UILayer::drawMainMenu(Application& app)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Configuration"))
            {
                JsonHelper::saveConfig(m_savePath, *app.m_currentFractal, app.m_camera);
            }
            if (ImGui::MenuItem("Load Configuration"))
            {
                JsonHelper::loadConfig(m_loadPath, *app.m_currentFractal, app.m_camera);
            }
            if (ImGui::MenuItem("Exit"))
            {
                glfwSetWindowShouldClose(app.m_window, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Fractal"))
        {
            drawFractalSelector(app);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void UILayer::drawFractalControls(IFractal* fractal)
{
    if (!fractal) return;

    ImGui::Begin("Fractal Parameters");
    fractal->drawGui();
    ImGui::End();
}

void UILayer::drawFractalSelector(Application& app)
{
    auto names = app.m_fractalRegistry.getNames();
    for (const auto& name : names)
    {
        if (ImGui::MenuItem(name.c_str()))
        {
            app.m_currentFractal = app.m_fractalRegistry.createFractal(name);
        }
    }
}
