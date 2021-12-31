#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    class Shader
    {
        enum class Type
        {
            Vertex,
            Fragment,
            Program
        };

    public:
        void Compile(const char* vertex_source, const char* fragment_source);
        void Use();
        unsigned int GetId();

        void SetInt(const char* name, int value);
        void SetFloat(const char* name, float value);
        void SetVec2(const char* name, float x, float y);
        void SetVec3(const char* name, float x, float y, float z);
        void SetVec4(const char* name, float x, float y, float z, float w);

        void SetMatrix4(const char* name, const glm::mat4& matrix);
    private:
        unsigned int id_;

    private:
        void CheckCompile(unsigned int id, Type type);
    };
}
#endif