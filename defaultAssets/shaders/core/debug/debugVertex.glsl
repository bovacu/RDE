#version 330 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_color;
out vec4 color;

uniform mat4 viewProjectionMatrix;

void main() {
    gl_Position = viewProjectionMatrix * vec4(in_position, 0.0, 1.0);
	color = in_color;
}
