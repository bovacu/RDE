#version 300 es

layout(location = 4) in vec3 position;
layout(location = 5) in vec4 color;
out vec4 color_from_vshader;

uniform mat4 viewProjectionMatrix;

void main() {
    gl_Position = viewProjectionMatrix * vec4(position, 1.0);
    color_from_vshader = color;
}
