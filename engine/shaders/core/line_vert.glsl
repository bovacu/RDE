#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
out vec4 color;

uniform mat4 view_projection_matrix;

void main() {
	gl_Position = view_projection_matrix * vec4(in_position, 1.0);
	color = in_color;
}