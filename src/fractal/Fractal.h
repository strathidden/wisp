#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann {
template<>
struct adl_serializer<glm::vec3> {
    static void to_json(json& j, const glm::vec3& v) {
        j = json::array({v.x, v.y, v.z});
    }
    
    static void from_json(const json& j, glm::vec3& v) {
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    }
};
}

struct FractalParams
{
    int iterations = 8;
    float power = 8.0f;
    glm::vec3 juliaSeed = {0.3f, 0.5f, 0.4f};
    glm::vec3 color = {0.8f, 0.4f, 0.2f};
    float glowStrength = 0.1f;
    float specular = 0.5f;
    float aoStrength = 0.5f;
    float shadowStrength = 0.7f;
    int maxSteps = 256;
    float stepScale = 0.8f;
    float minDistance = 0.001f;
    bool enableAO = true;
    bool enableShadows = true;
    bool enableGlow = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    FractalParams,
    iterations, 
    power, 
    juliaSeed, 
    color, 
    glowStrength, 
    specular,
    aoStrength,
    shadowStrength,
    maxSteps,
    stepScale,
    minDistance,
    enableAO,
    enableShadows,
    enableGlow
)
