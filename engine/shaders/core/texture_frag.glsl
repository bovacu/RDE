#version 330 core

header_2d_frag

vec2 uv9slice(vec2 uv, vec2 s, vec4 b) {
    vec2 t = clamp((s * uv - b.xy) / (s - b.xy - b.zw), 0., 1.);
	return mix(uv * s, 1. - s * (1. - uv), t);
}

vec4 draw_nine_slice() {
	vec2 _s = size.xy / texture_size * vec2(0.25, 1.0 - 0.75);
    vec4 _b = nine_slice / texture_size.xxyy;
    vec2 _uv = uv9slice(uv, _s, _b);
    vec3 _col = vec3(texture(tex, (vec2(0.25, 1.0 - 0.75) * _uv + vec2(0.0, 0.75))).x);
	return vec4(_col, 1.0);
}

vec4 draw_texture() {
	return texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}

// void main(void) {
// 	if(any(greaterThan(nine_slice, vec4(0)))) {
// 		out_color = draw_nine_slice();
// 	} else {
// 		out_color = draw_texture();
// 	}
// }

void main() {
	vec2 prop = vec2(sub_texture_uv.z - sub_texture_uv.x, sub_texture_uv.w - sub_texture_uv.y);
	vec2 offset = vec2(sub_texture_uv.x, sub_texture_uv.y);
	vec2 _uv = uv;
    vec2 ratio = (texture_size.xy / size.xy) * prop;
    vec2 border = vec2(nine_slice.x, nine_slice.z);
    vec2 bSrc = border / texture_size;
    vec2 bDst = border / size;

	// Left
    if(_uv.x < bDst.x) {
        _uv.x = _uv.x / ratio.x;
    } 

	// Right
	else if(_uv.x > (sub_texture_uv.z - bDst.x)) {
        _uv.x = (sub_texture_uv.z - _uv.x) / ratio.x;
    } 
	
	// Middle
	else {
        float t = (border.x * (texture_size.x - size.x)) / (texture_size.x * (2.0 * border.x - size.x));
        _uv.x = _uv.x * (1.0 - t * 2.0) + t;
    }
	
	// Down
    if(_uv.y < bDst.y + offset.y) {
		_uv.y = (_uv.y - offset.y) / ratio.y;
    } 
	
	// Up
	else if(_uv.y > (1.0 - bDst.y)) {
        _uv.y = 1.0 - (1.0 - _uv.y) / ratio.y;
    } 

	// Middle
	else {
        float t = (border.y * (texture_size.y - size.y)) / (texture_size.y * (2.0 * border.y - size.y));
        _uv.y = _uv.y * (1.0 - t * 2.0) + t;
    }

    out_color = texture(tex, (prop * _uv + offset));
}