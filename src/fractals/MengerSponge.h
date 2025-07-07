#pragma once

#include "IFractal.h"

class MengerSponge : public IFractal
{
public:
    MengerSponge() = default;
    
    void setUniforms(Shader& shader) const override;
    void drawGui() override;
    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json& json) override;
    std::string getName() const override { return "Menger Sponge"; }

private:
    int m_iterations = 3;
    glm::vec3 m_offset = {0.0f, 0.0f, 0.0f};
    float m_scale = 3.0f;
};
