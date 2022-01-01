#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;

void main()
{
    FragColor = mix(texture(firstTexture, texCoord), texture(secondTexture, texCoord), 0.2);
}