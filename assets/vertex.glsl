#version 400 core

// Vertex attributes for position and color
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;

uniform mat4 viewProjectionMatrix;

// output variables
out vec2 uv;
out vec4 color;

void main(void) {
	uv = in_uv;
	color = in_color;

	gl_Position = viewProjectionMatrix * vec4(in_position, 1);
}