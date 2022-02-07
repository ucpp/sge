#version 460 core
out vec4 FragColor;

in vec3 VertColor;

uniform vec3 color;

void main()
{
    FragColor = vec4(VertColor * color, 1.0);
}