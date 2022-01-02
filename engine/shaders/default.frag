#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;

void main()
{
    FragColor = texture(diffuse, texCoord);
}