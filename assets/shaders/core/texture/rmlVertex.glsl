#version 400 core

layout(location = 12) in vec2 inPosition;
layout(location = 13) in vec4 inColor0;
layout(location = 14) in vec2 inTexCoord0;

uniform vec2 position;
uniform mat4 viewProjectionMatrix;

out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
	fragTexCoord = inTexCoord0;
	fragColor = inColor0;
	vec2 translatedPos = inPosition + position.xy;
	vec4 outPos = viewProjectionMatrix * vec4(translatedPos, 0, 1);
	gl_Position = outPos;
}
