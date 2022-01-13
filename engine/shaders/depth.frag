#version 460 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(gl_FragDepth), 1.0);
}