#version 400 core

layout(location = 15) in vec2 in_position;
layout(location = 16) in vec4 in_color;
layout(location = 17) in vec2 in_uv;

uniform mat4 viewProjectionMatrix;

out vec2 uv;
out vec4 color;

void main(void) {
	uv = in_uv;
	color = in_color;
	gl_Position = viewProjectionMatrix * vec4(in_position, 0, 1);
}
