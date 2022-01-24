#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out VertexData
{
    vec3 position;
    vec2 texCoords;
    vec3 normal;
    vec3 tangent;
    vec4 lightSpacePosition;
} outData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main()
{
    outData.position = vec3(model * vec4(aPos, 1.0));
    outData.texCoords = aTexCoords;

    outData.normal = (model * vec4(aNormal, 0.0)).xyz;
    outData.tangent = (model * vec4(aTangent, 0.0)).xyz;

    outData.lightSpacePosition = lightSpace * vec4(outData.position, 1.0);
	gl_Position = projection * view * vec4(outData.position, 1.0);
}