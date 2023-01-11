#version 300 es

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
out vec4 color_from_vshader;

uniform mat4 viewProjectionMatrix;

void main() {
    gl_Position = viewProjectionMatrix * vec4(position, 0.0, 1.0);
    color_from_vshader = color;
}
