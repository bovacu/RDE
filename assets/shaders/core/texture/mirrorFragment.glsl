#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

void main(void) {
    vec4 _mirror = texture(tex, vec2(uv.x, 0.5 - uv.y)) * vec4(0, 0, 0, 1);
    gl_FragColor =  _mirror;
}