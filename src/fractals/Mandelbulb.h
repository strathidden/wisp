#pragma once

#include "IFractal.h"

class Mandelbulb : public IFractal
{
public:
    Mandelbulb() = default;
    
    void setUniforms(Shader& shader) const override;
    void drawGui() override;
    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& json) override;
    std::string getName() const override { return "Mandelbulb"; }

private:
    int m_iterations = 8;
    float m_power = 8.0f;
    float m_bailout = 2.0f;
    glm::vec3 m_offset = {0.0f, 0.0f, 0.0f};
};
