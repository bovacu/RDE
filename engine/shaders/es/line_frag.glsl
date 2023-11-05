#version 300 es
precision mediump float;

in vec4 color;
out vec4 out_color;

void main() {
	out_color = vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}