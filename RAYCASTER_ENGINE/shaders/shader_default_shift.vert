#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uvs;

out vec2 uvs;

uniform mat4 projection;
uniform mat4 model;
uniform vec2 uv_shift;

void main() {
	if (a_uvs.x == 1)
		uvs.x = 1 - (1 - uv_shift.y);
	else
		uvs.x = uv_shift.x;
	uvs.y = a_uvs.y;
	
	gl_Position = projection * model * vec4(a_pos, 1.0);
}