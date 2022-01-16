#include "shader.h"

#include <glad/glad.h>
#include <iostream>

namespace Engine
{
    void Shader::Use()
    {
        glUseProgram(GetId());
    }

    unsigned int Shader::GetId()
    {
        return id_;
    }

    void Shader::Compile(const std::string &vertex_source, const std::string &fragment_source)
    {
        unsigned int vertex;
        unsigned int fragment;
        this->vertex_source = vertex_source;
        this->fragment_source = fragment_source;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        const char *v_source = vertex_source.c_str();
        glShaderSource(vertex, 1, &v_source, nullptr);
        glCompileShader(vertex);
        CheckCompile(vertex, Type::Vertex);

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        const char *f_source = fragment_source.c_str();
        glShaderSource(fragment, 1, &f_source, nullptr);
        glCompileShader(fragment);
        CheckCompile(fragment, Type::Fragment);

        id_ = glCreateProgram();
        glAttachShader(id_, vertex);
        glAttachShader(id_, fragment);

        glLinkProgram(id_);
        CheckCompile(id_, Type::Program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Shader::SetBool(const char *name, bool value)
    {
        glUniform1i(glGetUniformLocation(id_, name), (int)value);
    }

    void Shader::SetInt(const char *name, int value)
    {
        glUniform1i(glGetUniformLocation(id_, name), value);
    }

    void Shader::SetFloat(const char *name, float value)
    {
        glUniform1f(glGetUniformLocation(id_, name), value);
    }

    void Shader::SetVec2(const char *name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(id_, name), x, y);
    }

    void Shader::SetVec3(const char *name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(id_, name), x, y, z);
    }

    void Shader::SetVec3(const char *name, glm::vec3 v3)
    {
        glUniform3f(glGetUniformLocation(id_, name), v3.x, v3.y, v3.z);
    }

    void Shader::SetVec4(const char *name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(id_, name), x, y, z, w);
    }

    void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, false, glm::value_ptr(matrix));
    }

    void Shader::CheckCompile(unsigned int id, Type type)
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