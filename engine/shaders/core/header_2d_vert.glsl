layout(location = 0) in vec2 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_uv;

uniform mat4 view_projection_matrix;
uniform float dt;
uniform vec2 mouse_position;

out vec2 uv;
out vec4 color;