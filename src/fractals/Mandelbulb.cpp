#include "Mandelbulb.h"
#include "../graphics/Shader.h"
#include <imgui.h>

void Mandelbulb::setUniforms(Shader& shader) const
{
    shader.setInt("fractalType", 0);
    shader.setInt("iterations", m_iterations);
    shader.setFloat("power", m_power);
    shader.setFloat("bailout", m_bailout);
    shader.setVec3("offset", m_offset);
}

void Mandelbulb::drawGui()
{
    ImGui::SliderInt("Iterations", &m_iterations, 1, 20);
    ImGui::SliderFloat("Power", &m_power, 1.0f, 20.0f);
    ImGui::SliderFloat("Bailout", &m_bailout, 1.0f, 10.0f);
    ImGui::SliderFloat3("Offset", &m_offset.x, -2.0f, 2.0f);
}

nlohmann::json Mandelbulb::toJson() const
{
    return {
        {"type", "Mandelbulb"},
        {"iterations", m_iterations},
        {"power", m_power},
        {"bailout", m_bailout},
        {"offset", {m_offset.x, m_offset.y, m_offset.z}}
    };
}

void Mandelbulb::fromJson(const nlohmann::json& json)
{
    if (json.contains("iterations")) m_iterations = json["iterations"];
    if (json.contains("power")) m_power = json["power"];
    if (json.contains("bailout")) m_bailout = json["bailout"];
    
    if (json.contains("offset") && json["offset"].is_array() && json["offset"].size() == 3)
    {
        m_offset.x = json["offset"][0];
        m_offset.y = json["offset"][1];
        m_offset.z = json["offset"][2];
    }
}
