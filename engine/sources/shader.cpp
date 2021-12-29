#include "shader.h"

#include <glad/glad.h>

void Shader::Use()
{
    glUseProgram(GetId());
}

unsigned int Shader::GetId()
{
    return id_;
}

void Shader::Compile(const char* vertex_source, const char* fragment_source)
{
    unsigned int vertex;
    unsigned int fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, nullptr);
    glCompileShader(vertex);
    // TODO write checks

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, nullptr);
    glCompileShader(fragment);
    // TODO write checks

    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);

    glLinkProgram(id_);
    //TODO write checks

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}