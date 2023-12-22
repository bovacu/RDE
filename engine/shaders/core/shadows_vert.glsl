#version 330 core

layout (location = 0) in vec3 in_pos;
layout(location = 3) in mat4 in_model;

uniform mat4 light_space_matrix;

void main() {
	gl_Position = light_space_matrix * in_model * vec4(in_pos, 1.0);
}
