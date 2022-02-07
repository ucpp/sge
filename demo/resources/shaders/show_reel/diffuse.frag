#version 460 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
};

in vec2 TexCoords;

uniform Material material;
uniform vec3 color;

void main()
{
    FragColor = texture(material.diffuse, TexCoords) * vec4(color, 1.0);
}