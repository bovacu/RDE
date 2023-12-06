#version 330 core

header_2d_frag

vec2 uv9slice(vec2 uv, vec2 s, vec4 b) {
    vec2 t = clamp((s * uv - b.xy) / (s - b.xy - b.zw), 0., 1.);
	return mix(uv * s, 1. - s * (1. - uv), t);
}

vec4 draw_nine_slice() {
	vec2 ts = vec2(textureSize(tex, 0));
	vec2 s = size.xy / ts;

    vec4 b = min(nine_slice / ts.xyxy, vec4(.499));
    vec2 _uv = uv9slice(uv, s, b);
    vec3 _col = vec3(texture(tex, _uv).x);
	return vec4(_col, 1.0);
}

vec4 draw_texture() {
	return texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}

void main(void) {
	if(all(greaterThan(nine_slice, vec4(0, 0, 0, 0)))) {
		out_color = draw_nine_slice();
	} else {
		out_color = draw_texture();
	}
}