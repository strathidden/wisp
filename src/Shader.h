#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int m_handle;

    Shader(const std::string vertexPath, const std::string fragmentPath);

    void use();

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setMat4(const std::string& name, glm::mat4 value);
    void setVec2(const std::string& name, glm::vec2 value);
    void setVec3(const std::string& name, glm::vec3 value);
    void setVec4(const std::string& name, glm::vec4 value);

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
