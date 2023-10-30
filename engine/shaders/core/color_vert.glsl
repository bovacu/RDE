#version 330 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;
flat out vec4 color;

uniform mat4 view_projection_matrix;

void main() {
	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);
	color = in_color;
}