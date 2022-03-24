#version 330
const vec2 quad_vertices[6] = vec2[6](vec2(-2.0, -2.0), vec2(2.0, -2.0), vec2(2.0, 2.0), vec2(-2.0, -2.0), vec2(2.0, 2.0), vec2(-2.0, 2.0));
uniform mat4 viewProjectionMatrix;

void main() {
//    gl_Position = viewProjectionMatrix * vec4(quad_vertices[gl_VertexID], 0.0, 1.0);
    gl_Position = vec4(quad_vertices[gl_VertexID], 0.0, 1.0);
}