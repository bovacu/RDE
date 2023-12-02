#version 330 core

header_3d_vert

void main(){
	mat4 _model = in_model;
	gl_Position = view_projection_matrix * _model * vec4(in_pos, 1);
	normal = in_normal;
	frag_pos = in_pos;
	text_coord = in_text_coord;
	model_matrix = _model;
}