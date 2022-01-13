#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

#define COUNT_POINT_LIGHTS 4

out VS_OUT
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec4 LightSpacePosition;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main()
{
    vs_out.Position = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

    vs_out.Normal = (model * vec4(aNormal, 0.0)).xyz;
    vs_out.Tangent = (model * vec4(aTangent, 0.0)).xyz;
    vs_out.LightSpacePosition = lightSpace * vec4(vs_out.Position, 1.0);
    gl_Position = projection * view * vec4(vs_out.Position, 1.0);
}