#include "UIManager.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <algorithm>
#include <format>
#include <array>

UIManager::UIManager(GLFWwindow* window, FractalParams& params)
    : m_fractalParams(params)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsLight();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.97f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.92f, 0.95f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.88f, 0.92f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.88f, 0.88f, 0.90f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.82f, 0.85f, 0.90f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.78f, 0.82f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.65f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.55f, 0.80f, 1.00f);
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(12, 12);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

UIManager::~UIManager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::render(float deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawMainWindow();
    if (m_showPerformance) drawPerformanceWindow(deltaTime);
    if (m_showFileDialog) drawFileDialog();
    if (m_showPresets) drawPresetsPanel();

    if (m_showColorPicker1)
    {
        ImGui::OpenPopup("Color Picker 1");
        m_showColorPicker1 = false;
    }

    if (m_showColorPicker2)
    {
        ImGui::OpenPopup("Color Picker 2");
        m_showColorPicker2 = false;
    }

    drawColorPicker("Color Picker 1", m_fractalParams.color1);
    drawColorPicker("Color Picker 2", m_fractalParams.color2);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool UIManager::wantCaptureMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool UIManager::wantCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

void UIManager::drawMainWindow()
{
    ImGui::SetNextWindowSize(ImVec2(420, 550), ImGuiCond_FirstUseEver);
    ImGui::Begin("Fractal Controls", nullptr, ImGuiWindowFlags_MenuBar);
    
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load"))
            {
                m_showFileDialog = true;
                m_isSaving = false;
            }
            if (ImGui::MenuItem("Save"))
            {
                m_showFileDialog = true;
                m_isSaving = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Performance", nullptr, &m_showPerformance);
            ImGui::MenuItem("Presets", nullptr, &m_showPresets);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    if (ImGui::CollapsingHeader("Fractal Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderInt("Max Iterations", &m_fractalParams.maxIterations, 1, 500);
        ImGui::SliderFloat("Power", &m_fractalParams.power, 1.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Bailout", &m_fractalParams.bailout, 0.1f, 10.0f, "%.2f");
    }
    
    if (ImGui::CollapsingHeader("Rendering Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::Text("Color 1:");
        ImGui::SameLine();
        ImVec4 col1 = ImVec4(m_fractalParams.color1[0], m_fractalParams.color1[1], m_fractalParams.color1[2], 1.0f);
        if (ImGui::ColorButton("##color1", col1))
        {
            m_showColorPicker1 = true;
        }
        
        ImGui::SameLine();
        ImGui::Text("Color 2:");
        ImGui::SameLine();
        ImVec4 col2 = ImVec4(m_fractalParams.color2[0], m_fractalParams.color2[1], m_fractalParams.color2[2], 1.0f);
        if (ImGui::ColorButton("##color2", col2))
        {
            m_showColorPicker2 = true;
        }
        
        ImGui::SliderFloat("Ambient", &m_fractalParams.ambient, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Diffuse", &m_fractalParams.diffuse, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Specular", &m_fractalParams.specular, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Shininess", &m_fractalParams.shininess, 1.0f, 256.0f, "%.0f");
        ImGui::SliderInt("Max Steps", &m_fractalParams.maxSteps, 32, 512);
        ImGui::SliderInt("AA Samples", &m_fractalParams.samples, 1, 16);
    }

    if (ImGui::Button("Reset to Defaults", ImVec2(-1, 0)))
    {
        m_fractalParams = FractalParams();
    }
    
    ImGui::End();
}

void UIManager::drawColorPicker(const std::string& label, glm::vec3& color)
{
    if (ImGui::BeginPopupModal(label.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        float col[3] = {color[0], color[1], color[2]};
        if (ImGui::ColorPicker3("##picker", col, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB))
        {
            color = {col[0], col[1], col[2]};
        }
        
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UIManager::drawFileDialog()
{
    ImGui::OpenPopup(m_isSaving ? "Save Config" : "Load Config");
    
    if (ImGui::BeginPopupModal(m_isSaving ? "Save Config" : "Load Config", &m_showFileDialog, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char filename[256] = "config.json";
        ImGui::InputText("Filename", filename, sizeof(filename));
        
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            if (m_isSaving)
            {
                m_configManager.saveConfig(filename, m_fractalParams);
            }
            else
            {
                m_configManager.loadConfig(filename, m_fractalParams);
            }
            m_showFileDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_showFileDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::Separator();
        ImGui::Text("Recent Files:");

        auto recentFiles = m_configManager.getRecentFiles();
        for (const auto& file : recentFiles)
        {
            if (ImGui::Selectable(file.c_str()))
            {
                #ifdef _WIN32
                strncpy_s(filename, sizeof(filename), file.c_str(), _TRUNCATE);
                #else
                strncpy(filename, file.c_str(), sizeof(filename) - 1);
                filename[sizeof(filename) - 1] = '\0';
                #endif
            }
        }

        ImGui::EndPopup();
    }
}

void UIManager::drawPerformanceWindow(float deltaTime)
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 120), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Performance", &m_showPerformance))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Frame time: %.3f ms", deltaTime * 1000.0f);
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);

    static std::array<float, 100> history{};
    static size_t offset = 0;
    
    history[offset] = deltaTime * 1000.0f;
    offset = (offset + 1) % history.size();

    std::string overlay = std::format("{:.1f} FPS", 1.0f / deltaTime);
    ImGui::PlotLines("Frame Times", history.data(), static_cast<int>(history.size()), static_cast<int>(offset), overlay.c_str(), 0.0f, 50.0f, ImVec2(0, 40));
    
    ImGui::End();
}

void UIManager::drawPresetsPanel()
{
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Presets", &m_showPresets);

    if (ImGui::Button("Mandelbulb", ImVec2(-FLT_MIN, 0)))
    {
        m_fractalParams = FractalParams();
    }

    if (ImGui::Button("High Detail", ImVec2(-FLT_MIN, 0)))
    {
        m_fractalParams.maxIterations = 200;
        m_fractalParams.minDistance = 0.0005f;
        m_fractalParams.samples = 4;
    }
    
    if (ImGui::Button("Colorful", ImVec2(-FLT_MIN, 0)))
    {
        m_fractalParams.color1 = {0.8f, 0.2f, 0.1f};
        m_fractalParams.color2 = {0.1f, 0.4f, 0.9f};
    }

    ImGui::End();
}
