in vec2 uv;
in vec4 color;
in vec2 size;
in vec4 nine_slice;

uniform sampler2D tex;
uniform float dt;
uniform vec2 mouse_position;

layout(location = 0) out vec4 out_color;