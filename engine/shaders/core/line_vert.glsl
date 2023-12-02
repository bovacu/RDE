#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;

uniform mat4 view_projection_matrix;
uniform float dt;
uniform vec2 mouse_position;

out vec2 uv;
out vec4 color;

void main() {
	gl_Position = view_projection_matrix * vec4(in_position, 1.0);
	color = in_color;
}