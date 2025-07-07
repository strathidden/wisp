#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

class Shader;

class IFractal
{
public:
    virtual ~IFractal() = default;

    virtual void setUniforms(Shader& shader) const = 0;
    virtual void drawGui() = 0;
    virtual nlohmann::json toJson() const = 0;
    virtual void fromJson(const nlohmann::json& json) = 0;
    virtual std::string getName() const = 0;
};
