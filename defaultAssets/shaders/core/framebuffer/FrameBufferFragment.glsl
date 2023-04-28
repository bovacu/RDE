#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;
layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(screenTexture, TexCoords);
}
