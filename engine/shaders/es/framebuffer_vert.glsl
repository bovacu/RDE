#version 300 es
precision mediump float;

header_2d_vert

void main() {
	gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
	uv = in_uv;
}
