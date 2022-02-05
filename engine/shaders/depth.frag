#version 460 core
out vec2 FragColor;
in vec4 v_position;

void main()
{
	float depth = v_position.z / v_position.w;
	depth = depth * 0.5 + 0.5;

	float moment1 = depth;
	float moment2 = depth * depth;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25 * (dx * dx + dy * dy);
	
	FragColor = vec2(moment1, moment2);
}