#version 400 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D _tex;

out vec4 finalColor;

void main() {
	vec4 texColor = texture(_tex, fragTexCoord);
	finalColor = fragColor * texColor;
}
