#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

void main(void) {
    gl_FragColor = vec4(1, 1, 1, texture2D(tex, uv).a) * color;
}