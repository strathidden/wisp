#include "IFS.h"
#include "../graphics/Shader.h"
#include <imgui.h>
#include <glm/gtc/constants.hpp>

void IFS::setUniforms(Shader& shader) const
{
    shader.setInt("fractalType", 2);
    shader.setInt("iterations", m_iterations);
    shader.setInt("pointsPerFrame", m_pointsPerFrame);
    shader.setFloat("scale", m_scale);
    shader.setVec3("offset", m_offset);
    shader.setVec3("rotation", m_rotation);
}

void IFS::drawGui()
{
    ImGui::SliderInt("Iterations", &m_iterations, 1000, 100000);
    ImGui::SliderInt("Points Per Frame", &m_pointsPerFrame, 100, 10000);
    ImGui::SliderFloat("Scale", &m_scale, 0.1f, 0.9f);
    ImGui::SliderFloat3("Offset", &m_offset.x, -2.0f, 2.0f);
    ImGui::SliderFloat3("Rotation", &m_rotation.x, -glm::pi<float>(), glm::pi<float>());
}

nlohmann::json IFS::toJson() const
{
    return {
        {"type", "IFS"},
        {"iterations", m_iterations},
        {"pointsPerFrame", m_pointsPerFrame},
        {"scale", m_scale},
        {"offset", {m_offset.x, m_offset.y, m_offset.z}},
        {"rotation", {m_rotation.x, m_rotation.y, m_rotation.z}}
    };
}

void IFS::fromJson(const nlohmann::json& json)
{
    if (json.contains("iterations")) m_iterations = json["iterations"];
    if (json.contains("pointsPerFrame")) m_pointsPerFrame = json["pointsPerFrame"];
    if (json.contains("scale")) m_scale = json["scale"];
    
    if (json.contains("offset") && json["offset"].is_array() && json["offset"].size() == 3)
    {
        m_offset.x = json["offset"][0];
        m_offset.y = json["offset"][1];
        m_offset.z = json["offset"][2];
    }
    
    if (json.contains("rotation") && json["rotation"].is_array() && json["rotation"].size() == 3)
    {
        m_rotation.x = json["rotation"][0];
        m_rotation.y = json["rotation"][1];
        m_rotation.z = json["rotation"][2];
    }
}
