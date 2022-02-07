#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 4) in vec3 aCol;

out vec3 VertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	VertColor = aCol;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}