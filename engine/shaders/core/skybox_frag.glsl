#version 330 core

in vec3 texcoords;
out vec4 color_out;

uniform samplerCube skybox;

void main() {
	color_out = texture(skybox, texcoords);
}