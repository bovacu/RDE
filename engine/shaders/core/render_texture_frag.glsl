#version 330 core

in vec3 normal;
in vec2 text_coord;
in vec3 frag_pos;
out vec4 color_out;
uniform sampler2D render_texture;

void main(){
	color_out = texture(render_texture, text_coord);
}
