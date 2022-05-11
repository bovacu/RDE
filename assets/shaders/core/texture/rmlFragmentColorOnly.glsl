#version 400

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

void main() {
    finalColor = fragColor;
}
