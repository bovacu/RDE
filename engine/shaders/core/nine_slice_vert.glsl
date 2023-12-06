#version 330 core

header_2d_vert

void main(void) {
	uv = in_uv;
	color = in_color;
	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);
} 