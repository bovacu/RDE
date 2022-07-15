#version 300 es

precision mediump float;

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

out vec4 outColor;

void main(void) {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, uv).a);
    outColor = color * sampled;
}