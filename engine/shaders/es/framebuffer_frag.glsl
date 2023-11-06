#version 300 es
precision mediump float;
in vec2 tex_coords;

uniform sampler2D screen_texture;
layout(location = 0) out vec4 out_color;

void main() {
	out_color = texture(screen_texture, tex_coords);
}