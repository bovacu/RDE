#version 330 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_text_coord;
layout(location = 3) in mat4 in_model;

out vec3 normal;
out vec2 text_coord;
out vec3 frag_pos;
out mat4 model_matrix;
uniform mat4 view_projection_matrix;

void main(){
	mat4 _model = in_model;
	gl_Position = view_projection_matrix * _model * vec4(in_pos, 1);
	normal = in_normal;
	frag_pos = in_pos;
	text_coord = in_text_coord;
	model_matrix = _model;
}