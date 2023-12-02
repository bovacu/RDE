in vec2 uv;
in vec4 color;

uniform sampler2D tex;
uniform float dt;
uniform vec2 mouse_position;

layout(location = 0) out vec4 out_color;