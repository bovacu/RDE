#version 400 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;

uniform mat4 viewProjectionMatrix;

out vec2 uv;
out vec4 color;

const float PI = 3.14159;

void main(void) {

    int _vertex = gl_VertexID % 6;
    float _angle = 5 * PI / 180.0;
    if(_vertex == 0 || _vertex == 1 || _vertex == 3){
        uv = vec2(in_uv.x + 0.1, in_uv.y - 0.4);
    }  else
        uv = in_uv;

    color = in_color;
    gl_Position = viewProjectionMatrix * vec4(in_position.x, in_position.y, in_position.z, 1);
}
