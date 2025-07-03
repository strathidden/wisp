#pragma once
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "../fractal/Fractal.h"
#include "../fractal/Julia.h"
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <expected>
#include <nlohmann/json.hpp>

class UIManager
{
public:
    UIManager(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        setupStyle();
        loadPresetList();
    }

    ~UIManager()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void beginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void endFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    template<typename T>
    void renderFractalControls(T& fractal, const char* name)
    {
        if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(200);
            ImGui::SliderInt("Iterations", &fractal.params.iterations, 1, 30);
            ImGui::SliderFloat("Power", &fractal.params.power, 1.0f, 20.0f);
            
            if constexpr (std::is_same_v<T, Julia>)
            {
                ImGui::SliderFloat3("Julia Seed", &fractal.params.juliaSeed.x, -2.0f, 2.0f);
            }
            
            ImGui::ColorEdit3("Color", &fractal.params.color.r);
            ImGui::SliderFloat("Glow Strength", &fractal.params.glowStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &fractal.params.specular, 0.0f, 1.0f);
            ImGui::SliderFloat("AO Strength", &fractal.params.aoStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Shadow Strength", &fractal.params.shadowStrength, 0.0f, 1.0f);
            ImGui::SliderInt("Max Steps", &fractal.params.maxSteps, 16, 512);
            ImGui::SliderFloat("Step Scale", &fractal.params.stepScale, 0.1f, 1.0f);
            ImGui::SliderFloat("Min Distance", &fractal.params.minDistance, 0.0001f, 0.1f);
            
            ImGui::Checkbox("Enable AO", &fractal.params.enableAO);
            ImGui::SameLine();
            ImGui::Checkbox("Enable Shadows", &fractal.params.enableShadows);
            ImGui::SameLine();
            ImGui::Checkbox("Enable Glow", &fractal.params.enableGlow);
            
            ImGui::PopItemWidth();
        }
    }

    void renderPerformance(float fps, float frameTime)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = work_pos.x + work_size.x - PAD;
        window_pos.y = work_pos.y + PAD;
        window_pos_pivot.x = 1.0f;
        window_pos_pivot.y = 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Performance", nullptr, flags))
        {
            ImGui::Text("FPS: %.1f (%.2f ms)", fps, frameTime);
        }
        ImGui::End();
    }

    void renderSaveLoadModal(bool& openSave, bool& openLoad, const std::string& currentFractalName, const FractalParams& currentParams)
    {
        static char saveFilename[256] = "my_preset";
        static char loadFilename[256] = "";
        static int currentPreset = -1;
        
        if (openSave)
        {
            ImGui::OpenPopup("Save Configuration");
            openSave = false;
            strcpy_s(saveFilename, "my_preset");
        }

        if (openLoad)
        {
            ImGui::OpenPopup("Load Configuration");
            openLoad = false;
            currentPreset = -1;
        }

        if (ImGui::BeginPopupModal("Save Configuration", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Preset Name", saveFilename, sizeof(saveFilename));
            
            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                saveConfig(saveFilename, currentFractalName, currentParams);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Load Configuration", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::BeginListBox("Presets", ImVec2(-FLT_MIN, 150)))
            {
                for (int i = 0; i < m_presetNames.size(); i++)
                {
                    const bool isSelected = (currentPreset == i);
                    if (ImGui::Selectable(m_presetNames[i].c_str(), isSelected))
                    {
                        currentPreset = i;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }
            
            if (currentPreset != -1)
            {
                strcpy_s(loadFilename, m_presetNames[currentPreset].c_str());
            }
            
            ImGui::InputText("Filter", loadFilename, sizeof(loadFilename));
            
            if (ImGui::Button("Load", ImVec2(120, 0)) && currentPreset != -1)
            {
                auto result = loadConfig(m_presetNames[currentPreset]);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    const std::vector<std::string>& getPresetNames() const { return m_presetNames; }

private:
    std::vector<std::string> m_presetNames;

    void setupStyle()
    {
        ImGui::StyleColorsDark();
        
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(8, 6);
        style.CellPadding = ImVec2(4, 2);
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing = 20;
        style.ScrollbarSize = 14;
        style.GrabMinSize = 12;
        
        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 0;
        style.TabBorderSize = 0;
        
        style.WindowRounding = 8;
        style.ChildRounding = 8;
        style.FrameRounding = 6;
        style.PopupRounding = 8;
        style.ScrollbarRounding = 9;
        style.GrabRounding = 6;
        style.TabRounding = 6;
        
        style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.12f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.25f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.22f, 0.27f, 0.54f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.60f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.46f, 0.46f, 0.67f, 0.69f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.78f, 0.00f, 0.40f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.78f, 0.00f, 0.76f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.36f, 0.41f, 0.54f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.48f, 0.71f, 0.86f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.58f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.35f, 0.42f, 0.61f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.48f, 0.71f, 0.86f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.58f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    }
    
    void loadPresetList()
    {
        m_presetNames.clear();
        
        const std::filesystem::path presetDir = "config/presets";
        if (!std::filesystem::exists(presetDir))
        {
            std::filesystem::create_directories(presetDir);
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(presetDir))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                m_presetNames.push_back(entry.path().stem().string());
            }
        }
        
        std::ranges::sort(m_presetNames);
    }
    
    void saveConfig(const std::string& name, const std::string& fractalName, const FractalParams& params)
    {
        const std::filesystem::path presetDir = "config/presets";
        if (!std::filesystem::exists(presetDir))
        {
            std::filesystem::create_directories(presetDir);
        }
        
        nlohmann::json j;
        j["fractal"] = fractalName;
        j["params"] = params;
        
        std::ofstream file(presetDir / (name + ".json"));
        if (file)
        {
            file << j.dump(4);
            loadPresetList(); // Refresh list
        }
    }
    
    std::expected<FractalParams, std::string> loadConfig(const std::string& name)
    {
        const std::filesystem::path filePath = std::filesystem::path("config/presets") / (name + ".json");
        if (!std::filesystem::exists(filePath))
        {
            return std::unexpected("File not found");
        }
        
        try
        {
            std::ifstream file(filePath);
            nlohmann::json j;
            file >> j;
            return j["params"].get<FractalParams>();
        }
        catch (const std::exception& e)
        {
            return std::unexpected(e.what());
        }
    }
};
