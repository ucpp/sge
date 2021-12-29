#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
    void Compile(const char* vertex_source, const char* fragment_source);
    void Use();
    unsigned int GetId();

private:
    unsigned int id_;
};

#endif