#type vertex
#version 460 core

layout(location = 0) in vec3    position_lay;
layout(location = 1) in vec4    color_lay;
layout(location = 2) in vec2    texCoord_lay;
layout(location = 3) in float   texIndex_lay;
layout(location = 4) in float   tilingFactor_lay;

uniform mat4 viewProjection_uniform;

out vec4    color_vertex;
out vec2    texCoord_vertex;
out float   texIndex_vertex;
out float   tilingFactor_vertex;

void main() {
    color_vertex = color_lay;
    texCoord_vertex = texCoord_lay;
    texIndex_vertex = texIndex_lay;
    tilingFactor_vertex = tilingFactor_lay;
    gl_Position = viewProjection_uniform * vec4(position_lay, 1.0);
}

#type fragment
#version 460 core

uniform sampler2D textures_uniform[32];

in vec4 color_vertex;
in vec2 texCoord_vertex;
in float texIndex_vertex;
in float tilingFactor_vertex;

layout(location = 0) out vec4 color;

void main() {
    color = texture(textures_uniform[int(texIndex_vertex)], texCoord_vertex * tilingFactor_vertex) * color_vertex;
}