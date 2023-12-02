#version 330 core

header_2d_frag

void main() {
	out_color = vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}