#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

Shader::~Shader()
{
    if (m_id)
    {
        glDeleteProgram(m_id);
    }
}

std::expected<Shader, std::string> Shader::createCompute(const std::filesystem::path& path)
{
    auto source = loadShaderFile(path);
    if (!source)
    {
        return std::unexpected(source.error());
    }

    auto shader = compileShader(GL_COMPUTE_SHADER, *source);
    if (!shader)
    {
        return std::unexpected(shader.error());
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, *shader);
    glLinkProgram(program);

    glDeleteShader(*shader);

    Shader result(program);
    result.checkLinkErrors();
    return result;
}

std::expected<Shader, std::string> Shader::createVertexFragment(const std::filesystem::path &vertPath, const std::filesystem::path &fragPath)
{
    auto vertSource = loadShaderFile(vertPath);
    if (!vertSource)
    {
        return std::unexpected(vertSource.error());
    }

    auto fragSource = loadShaderFile(fragPath);
    if (!fragSource)
    {
        return std::unexpected(fragSource.error());
    }

    auto vertShader = compileShader(GL_VERTEX_SHADER, *vertSource);
    if (!vertShader)
    {
        return std::unexpected(vertShader.error());
    }
    
    auto fragShader = compileShader(GL_FRAGMENT_SHADER, *fragSource);
    if (!fragShader)
    {
        glDeleteShader(*vertShader);
        return std::unexpected(fragShader.error());
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, *vertShader);
    glAttachShader(program, *fragShader);
    glLinkProgram(program);

    glDeleteShader(*vertShader);
    glDeleteShader(*fragShader);

    Shader result(program);
    result.checkLinkErrors();
    return result;
}

void Shader::use() const
{
    glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

std::expected<std::string, std::string> Shader::loadShaderFile(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file)
    {
        return std::unexpected("Failed to open file: " + path.string());
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::expected<GLuint, std::string> Shader::compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        glDeleteShader(shader);
        return std::unexpected("Shader compilation failed: " + std::string(infoLog));
    }
    
    return shader;
}

void Shader::checkLinkErrors() const
{
    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_id, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << "\n";
    }
}
