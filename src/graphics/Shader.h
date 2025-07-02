#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <expected>
#include <filesystem>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool load(std::string_view vertPath, std::string_view fragPath);
    bool reload();
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    const std::string& getLastError() const { return m_lastError; }
    unsigned int getID() const { return m_id; }

private:
    GLuint m_id = 0;
    std::filesystem::path m_vertPath;
    std::filesystem::path m_fragPath;
    std::string m_lastError;
    mutable std::unordered_map<std::string, unsigned int> m_uniformCache;

    std::expected<GLuint, std::string> compileShader(GLenum type, const std::string& source);
    GLint getUniformLocation(const std::string& name) const;
};
