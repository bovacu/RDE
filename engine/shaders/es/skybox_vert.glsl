#version 300 es
precision mediump float;

layout(location = 0) in vec3 in_pos;
out vec3 texcoords;

uniform mat4 view_projection_matrix;

void main() {
	texcoords = in_pos;
	gl_Position = (view_projection_matrix * vec4(in_pos, 1)).xyww;
}