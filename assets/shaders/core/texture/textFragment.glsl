#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(1, 1, 1, texture(tex, uv).a) * color;
}