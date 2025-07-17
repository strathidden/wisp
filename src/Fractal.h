#pragma once
#include <glm/glm.hpp>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct FractalParams
{
    int maxIterations = 50;
    float power = 8.0f;
    float bailout = 2.0f;
    float scale = 1.5f;
    glm::vec3 offset = glm::vec3(0.0f);

    float minDistance = 0.001f;
    float maxDistance = 100.0f;
    glm::vec3 color1 = glm::vec3(0.2, 0.4f, 0.8f);
    glm::vec3 color2 = glm::vec3(0.8f, 0.4f, 0.2f);
    float ambient = 0.2f;
    float diffuse = 0.7f;
    float specular = 0.5f;
    float shininess = 32.0f;
    int maxSteps = 100;
    int samples = 1;

    void loadFromJson(const json& j);
    json saveToJson() const;
};

void to_json(json& j, const FractalParams& p);
void from_json(const json& j, FractalParams& p);

namespace nlohmann {
    template <>
    struct adl_serializer<FractalParams> {
        static void to_json(json& j, const FractalParams& p)
        {
            j = json{
                {"maxIterations", p.maxIterations},
                {"power", p.power},
                {"bailout", p.bailout},
                {"scale", p.scale},
                {"offset", {p.offset.x, p.offset.y, p.offset.z}},
                {"minDistance", p.minDistance},
                {"maxDistance", p.maxDistance},
                {"color1", {p.color1.x, p.color1.y, p.color1.z}},
                {"color2", {p.color2.x, p.color2.y, p.color2.z}},
                {"ambient", p.ambient},
                {"diffuse", p.diffuse},
                {"specular", p.specular},
                {"shininess", p.shininess},
                {"maxSteps", p.maxSteps},
                {"samples", p.samples}
            };
        }

        static void from_json(const json& j, FractalParams& p)
        {
            j.at("maxIterations").get_to(p.maxIterations);
            j.at("power").get_to(p.power);
            j.at("bailout").get_to(p.bailout);
            j.at("scale").get_to(p.scale);
            
            auto offset = j.at("offset").get<std::vector<float>>();
            p.offset = glm::vec3(offset[0], offset[1], offset[2]);
            
            j.at("minDistance").get_to(p.minDistance);
            j.at("maxDistance").get_to(p.maxDistance);
            
            auto color1 = j.at("color1").get<std::vector<float>>();
            p.color1 = glm::vec3(color1[0], color1[1], color1[2]);
            
            auto color2 = j.at("color2").get<std::vector<float>>();
            p.color2 = glm::vec3(color2[0], color2[1], color2[2]);
            
            j.at("ambient").get_to(p.ambient);
            j.at("diffuse").get_to(p.diffuse);
            j.at("specular").get_to(p.specular);
            j.at("shininess").get_to(p.shininess);
            j.at("maxSteps").get_to(p.maxSteps);
            j.at("samples").get_to(p.samples);
        }
    };
}
