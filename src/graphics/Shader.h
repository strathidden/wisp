#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <expected>
#include <filesystem>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    [[nodiscard]] static std::expected<Shader, std::string> createCompute(const std::filesystem::path& path);

    [[nodiscard]] static std::expected<Shader, std::string> createVertexFragment(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);

    void use() const;
    GLuint getID() const { return m_id; }
    operator bool() const { return m_id != 0; }

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_id = 0;

    Shader(GLuint program) : m_id(program) {}

    [[nodiscard]] static std::expected<GLuint, std::string> compileShader(GLenum type, const std::string& source);

    [[nodiscard]] static std::expected<std::string, std::string> loadShaderFile(const std::filesystem::path& path);

    void checkLinkErrors() const;
};
