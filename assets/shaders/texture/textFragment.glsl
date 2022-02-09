#version 300 es

precision mediump float;

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

out vec4 outColor;

void main(void) {
    outColor = vec4(1, 1, 1, texture2D(tex, uv).a) * color;
}