#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader
{
public:
    Shader() : m_id(0) {}
    ~Shader() { glDeleteProgram(m_id); }

    void load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const { glUseProgram(m_id); }

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
private:
    unsigned int m_id;
    
    std::string readFile(const std::string& filepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
};
