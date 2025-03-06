#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string vertexPath, const std::string fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    m_handle = glCreateProgram();
    glAttachShader(m_handle, vertex);
    glAttachShader(m_handle, fragment);
    glLinkProgram(m_handle);
    checkCompileErrors(m_handle, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(m_handle);
}

void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(m_handle, name.c_str()), (int)value);
}


void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(m_handle, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(m_handle, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_handle, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec2(const std::string& name, glm::vec2 value)
{
    glUniform2fv(glGetUniformLocation(m_handle, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, glm::vec3 value)
{
    glUniform3fv(glGetUniformLocation(m_handle, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, glm::vec4 value)
{
    glUniform4fv(glGetUniformLocation(m_handle, name.c_str()), 1, &value[0]);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
