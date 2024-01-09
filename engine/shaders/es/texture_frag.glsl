#version 300 es
precision mediump float;

header_2d_frag

void main(void) {
	out_color = texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}
