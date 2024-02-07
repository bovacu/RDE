layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec2 in_size;
layout(location = 4) in vec4 in_nine_slice;
layout(location = 5) in vec4 in_sub_texture_uv;

uniform mat4 view_projection_matrix;
uniform float dt;
uniform vec2 mouse_position;

out vec2 uv;
out vec4 color;
out vec2 size;
out vec4 nine_slice;
out vec4 sub_texture_uv;