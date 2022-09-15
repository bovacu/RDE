#version 330 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 out_color;

void main(void) {
    vec4 _mirror = texture(tex, vec2(uv.x, 0.5 - uv.y)) * vec4(0, 0, 0, 1);
    out_color =  _mirror;
}
