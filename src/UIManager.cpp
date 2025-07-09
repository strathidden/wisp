#include "UIManager.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <algorithm>

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
    colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.85f, 0.87f, 1.00f);
    
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
    if (m_showFileDialog) drawFileDialog();

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
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Once);
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
        ImGui::EndMenuBar();
    }
    
    if (ImGui::CollapsingHeader("Fractal Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderInt("Max Iterations", &m_fractalParams.maxIterations, 1, 500);
        ImGui::SliderFloat("Power", &m_fractalParams.power, 1.0f, 20.0f);
        ImGui::SliderFloat("Bailout", &m_fractalParams.bailout, 0.1f, 10.0f);
        ImGui::SliderFloat("Scale", &m_fractalParams.scale, 0.1f, 4.0f);
        ImGui::DragFloat3("Offset", &m_fractalParams.offset[0], 0.01f);
        ImGui::DragFloat3("Julia", &m_fractalParams.julia[0], 0.01f, -2.0f, 2.0f);
    }
    
    if (ImGui::CollapsingHeader("Rendering Parameters"))
    {
        ImGui::SliderFloat("Step Size", &m_fractalParams.stepSize, 0.001f, 0.1f);
        ImGui::SliderFloat("Min Distance", &m_fractalParams.minDistance, 0.0001f, 0.01f);
        ImGui::SliderFloat("Max Distance", &m_fractalParams.maxDistance, 10.0f, 200.0f);
        ImGui::ColorEdit3("Color 1", &m_fractalParams.color1[0]);
        ImGui::ColorEdit3("Color 2", &m_fractalParams.color2[0]);
        ImGui::SliderFloat("Ambient", &m_fractalParams.ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &m_fractalParams.diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &m_fractalParams.specular, 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &m_fractalParams.shininess, 1.0f, 256.0f);
    }
    
    ImGui::End();
}

void UIManager::drawFileDialog()
{
    ImGui::OpenPopup(m_isSaving ? "Save Config" : "Load Config");
    
    if (ImGui::BeginPopupModal(m_isSaving ? "Save Config" : "Load Config", &m_showFileDialog))
    {
        static char filename[128] = "config.json";
        ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));
        
        if (ImGui::Button("Confirm"))
        {
            if (m_isSaving)
            {
                m_configManager.saveConfig(filename, m_fractalParams);
                m_recentFiles.push_back(filename);
            }
            else
            {
                m_configManager.loadConfig(filename, m_fractalParams);
                if (std::find(m_recentFiles.begin(), m_recentFiles.end(), filename) == m_recentFiles.end())
                {
                    m_recentFiles.push_back(filename);
                }
            }
            m_showFileDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            m_showFileDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        if (!m_recentFiles.empty())
        {
            ImGui::Separator();
            ImGui::Text("Recent Files:");
            for (const auto& file : m_recentFiles)
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
        }
        
        ImGui::EndPopup();
    }
}
