#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 out_color;

void main(void) {
    out_color = vec4(1, 1, 1, texture2D(tex, uv).a) * color;
}