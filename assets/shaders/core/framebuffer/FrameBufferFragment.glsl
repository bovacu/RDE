#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    gl_FragColor = texture(screenTexture, TexCoords);
}
