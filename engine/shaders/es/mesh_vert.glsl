#version 300 es
precision mediump float;

header_3d_vert

void main(){
	mat4 _model = in_model;
	gl_Position = view_projection_matrix * _model * vec4(in_pos, 1);
	normal = in_normal;
	frag_pos = in_pos;
	text_coord = in_text_coord;
	model_matrix = _model;

	if(use_shadows == 1) {
		vs_out.frag_pos = vec3(in_model * vec4(in_pos, 1.0));
		vs_out.normal = transpose(inverse(mat3(in_model))) * normal;
		vs_out.frag_pos_light_space = light_space_matrix * vec4(vs_out.frag_pos, 1.0);
	}
}