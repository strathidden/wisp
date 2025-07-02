#include "Shader.h"
#include "../core/Logger.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::~Shader()
{
    if (m_id)
    {
        glDeleteProgram(m_id);
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_id(other.m_id),
    m_vertPath(std::move(other.m_vertPath)),
    m_fragPath(std::move(other.m_fragPath)),
    m_uniformCache(std::move(other.m_uniformCache)),
    m_lastError(std::move(other.m_lastError))
{
    other.m_id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (m_id)
        {
            glDeleteProgram(m_id);
        }
        m_id = other.m_id;
        m_vertPath = std::move(other.m_vertPath);
        m_fragPath = std::move(other.m_fragPath);
        m_uniformCache = std::move(other.m_uniformCache);
        m_lastError = std::move(other.m_lastError);
        other.m_id = 0;
    }
    return *this;
}

bool Shader::load(std::string_view vertPath, std::string_view fragPath)
{
    m_vertPath = vertPath;
    m_fragPath = fragPath;
    return reload();
}

bool Shader::reload()
{
    std::string vertCode;
    std::ifstream vShaderFile(m_vertPath);
    if (vShaderFile)
    {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertCode = vShaderStream.str();
        vShaderFile.close();
    }
    else
    {
        m_lastError = "Failed to open vertex shader: " + m_vertPath.string();
        return false;
    }

    std::string fragCode;
    std::ifstream fShaderFile(m_fragPath);
    if (fShaderFile)
    {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragCode = fShaderStream.str();
        fShaderFile.close();
    }
    else
    {
        m_lastError = "Failed to open fragment shader: " + m_fragPath.string();
        return false;
    }

    auto vertex = compileShader(GL_VERTEX_SHADER, vertCode);
    if (!vertex)
    {
        m_lastError = vertex.error();
        return false;
    }

    auto fragment = compileShader(GL_FRAGMENT_SHADER, fragCode);
    if (!fragment)
    {
        glDeleteShader(*vertex);
        m_lastError = fragment.error();
        return false;
    }

    GLuint id = glCreateProgram();
    glAttachShader(id, *vertex);
    glAttachShader(id, *fragment);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(*vertex);
        glDeleteShader(*fragment);
        glDeleteProgram(id);
        m_lastError = "Shader linking failed: " + std::string(infoLog);
        return false;
    }

    if (m_id)
    {
        glDeleteProgram(m_id);
    }
    m_id = id;

    glDeleteShader(*vertex);
    glDeleteShader(*fragment);

    m_uniformCache.clear();
    return true;
}

std::expected<GLuint, std::string> Shader::compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(shader);
        return std::unexpected("Shader compilation failed: " + std::string(infoLog));
    }
    return shader;
}

void Shader::use() const
{
    glUseProgram(m_id);
}

GLint Shader::getUniformLocation(const std::string& name) const
{
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end())
    {
        return it->second;
    }

    GLint location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1)
    {
        LOG_WARNING("Uniform '{}' not found", name);
    }

    m_uniformCache[name] = location;
    return location;
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
