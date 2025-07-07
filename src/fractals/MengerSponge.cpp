#include "MengerSponge.h"
#include "../graphics/Shader.h"
#include <imgui.h>

void MengerSponge::setUniforms(Shader& shader) const
{
    shader.setInt("fractalType", 1);
    shader.setInt("iterations", m_iterations);
    shader.setVec3("offset", m_offset);
    shader.setFloat("scale", m_scale);
}

void MengerSponge::drawGui()
{
    ImGui::SliderInt("Iterations", &m_iterations, 1, 5);
    ImGui::SliderFloat3("Offset", &m_offset.x, -2.0f, 2.0f);
    ImGui::SliderFloat("Scale", &m_scale, 1.0f, 5.0f);
}

nlohmann::json MengerSponge::toJson() const
{
    return {
        {"type", "MengerSponge"},
        {"iterations", m_iterations},
        {"offset", {m_offset.x, m_offset.y, m_offset.z}},
        {"scale", m_scale}
    };
}

void MengerSponge::fromJson(const nlohmann::json& json)
{
    if (json.contains("iterations")) m_iterations = json["iterations"];
    if (json.contains("scale")) m_scale = json["scale"];
    
    if (json.contains("offset") && json["offset"].is_array() && json["offset"].size() == 3)
    {
        m_offset.x = json["offset"][0];
        m_offset.y = json["offset"][1];
        m_offset.z = json["offset"][2];
    }
}
