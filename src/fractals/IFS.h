#pragma once

#include "IFractal.h"

class IFS : public IFractal
{
public:
    IFS() = default;
    
    void setUniforms(Shader& shader) const override;
    void drawGui() override;
    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& json) override;
    std::string getName() const override { return "IFS"; }

private:
    int m_iterations = 10000;
    int m_pointsPerFrame = 1000;
    float m_scale = 0.5f;
    glm::vec3 m_offset = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_rotation = {0.0f, 0.0f, 0.0f};
};
