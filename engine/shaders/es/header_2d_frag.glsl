in vec2 uv;
in vec4 color;
in vec2 size;
in vec4 nine_slice;
in vec4 sub_texture_uv;

uniform sampler2D tex;
uniform float dt;
uniform vec2 mouse_position;
uniform vec2 texture_size; // If drawing a subtexture from an atlas, this is the size of the subtexture

layout(location = 0) out vec4 out_color;
