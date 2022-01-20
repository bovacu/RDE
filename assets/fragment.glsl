#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

void main(void) {
	gl_FragColor = texture(tex, uv) * color;
}