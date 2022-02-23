#version 300 es

precision mediump float;

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

out vec4 outColor;

void main(void) {
	outColor = texture(tex, uv) * color;
}