#include "shader.h"

#include <glad/glad.h>
#include <iostream>

namespace sge
{
    void Shader::use()
    {
        glUseProgram(getId());
    }

    uint32_t Shader::getId()
    {
        return id;
    }

    void Shader::compile(const std::string &vertex_source, const std::string &fragment_source)
    {
        uint32_t vertex;
        uint32_t fragment;
        this->vertex_source = vertex_source;
        this->fragment_source = fragment_source;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        const char *v_source = this->vertex_source.c_str();
        glShaderSource(vertex, 1, &v_source, nullptr);
        glCompileShader(vertex);
        checkCompile(vertex, Type::Vertex);

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        const char *f_source = this->fragment_source.c_str();
        glShaderSource(fragment, 1, &f_source, nullptr);
        glCompileShader(fragment);
        checkCompile(fragment, Type::Fragment);

        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);

        glLinkProgram(id);
        checkCompile(id, Type::Program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Shader::setBool(const char *name, bool value)
    {
        glUniform1i(glGetUniformLocation(id, name), (int)value);
    }

    void Shader::setInt(const char *name, int value)
    {
        glUniform1i(glGetUniformLocation(id, name), value);
    }

    void Shader::setFloat(const char *name, float value)
    {
        glUniform1f(glGetUniformLocation(id, name), value);
    }

    void Shader::setVec2(const char *name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(id, name), x, y);
    }

    void Shader::setVec3(const char *name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(id, name), x, y, z);
    }

    void Shader::setVec3(const char *name, glm::vec3 v3)
    {
        glUniform3f(glGetUniformLocation(id, name), v3.x, v3.y, v3.z);
    }

    void Shader::setVec4(const char *name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
    }

    void Shader::setMatrix4(const char *name, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, glm::value_ptr(matrix));
    }

    void Shader::checkCompile(uint32_t id, Type type)
    {
        int success;
        const int buff_size = 512;
        char info[buff_size];

        if (type == Type::Program)
        {
            glGetShaderiv(id, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(id, buff_size, nullptr, info);
                std::cout << "Shader link time error:" << info << std::endl;
            }
        }
        else
        {
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(id, buff_size, nullptr, info);
                std::cout << "Shader compile error:" << info << std::endl;
            }
        }
    }
}