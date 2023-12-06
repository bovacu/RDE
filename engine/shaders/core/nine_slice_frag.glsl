#version 330 core

header_2d_frag

// Image 9-slicing in fragment shader.
// Feel free to share and use anywhere.
// Graph that I used to reason about the problem:
// https://www.desmos.com/calculator/wkvma9uh0a

// borders in pixels, x = left, y = bottom, z = right, w = top
#define B vec4(10, 10, 10, 10)
#define size vec2(128, 64)

vec2 uv9slice(vec2 uv, vec2 s, vec4 b) {
    vec2 t = clamp((s * uv - b.xy) / (s - b.xy - b.zw), 0., 1.);
	return mix(uv * s, 1. - s * (1. - uv), t);
}

void main(void) {
    vec2 ts = vec2(textureSize(tex, 0));
	vec2 s = size.xy / ts;

    vec4 b = min(B / ts.xyxy, vec4(.499));
    vec2 _uv = uv9slice(uv, s, b);
    vec3 col = vec3(texture(tex, _uv).x);

    out_color = vec4(col,1.0);
}