#version 330 core

header_2d_frag

vec4 draw_nine_slice() {
		// This is just to make the image scale correctly
	vec2 prop = vec2(sub_texture_uv.z - sub_texture_uv.x, sub_texture_uv.w - sub_texture_uv.y);

	// Point of start if sub-texture
	vec2 offset = vec2(sub_texture_uv.x, sub_texture_uv.y);
	vec2 _uv = uv;
    vec2 ratio = (texture_size.xy / size.xy);
    vec2 border = vec2(nine_slice.x, nine_slice.z);
    vec2 border_custom_prop = (border / size) * prop;
    vec2 border_original_prop = (border / texture_size) * prop;
	vec2 texture_coord = vec2(0);
	vec2 atlas_offset = vec2(0);

	// Left
    if(_uv.x < offset.x + border_custom_prop.x) {
        texture_coord.x = (_uv.x - offset.x) / ratio.x;
		atlas_offset.x = sub_texture_uv.x;
    } 

	// Right
	else if(_uv.x > (sub_texture_uv.z - border_custom_prop.x)) {
		texture_coord.x = (_uv.x - sub_texture_uv.z) / ratio.x;
		atlas_offset.x = sub_texture_uv.z;
	}
	
	// Middle
	else {
		texture_coord.x = _uv.x + (border_original_prop.x - border_custom_prop.x);
		if(texture_coord.x > sub_texture_uv.z - border_original_prop.x) {
			texture_coord.x = sub_texture_uv.z - border_original_prop.x;
		}
	}
	
	// Down
    if(_uv.y < offset.y + border_custom_prop.y) {
		texture_coord.y = (_uv.y - offset.y) / ratio.y;
		atlas_offset.y = sub_texture_uv.y;
    }
	
	// Up
	else if(_uv.y > (sub_texture_uv.w - border_custom_prop.y)) {
		texture_coord.y = (_uv.y - sub_texture_uv.w) / ratio.y;
		atlas_offset.y = sub_texture_uv.w;
	}

	// Middle
	else {
		texture_coord.y = _uv.y + border_original_prop.y - border_custom_prop.y;
		if(texture_coord.y > sub_texture_uv.w - border_original_prop.y) {
			texture_coord.y = sub_texture_uv.w - border_original_prop.y;
		}
	}

	vec4 _color = texture(tex, texture_coord + atlas_offset);
	if(_color.w < 0.05) {
		discard;
	}

    return texture(tex, texture_coord + atlas_offset) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}

vec4 draw_texture() {
	return texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}

void main() {
	if(any(greaterThan(nine_slice, vec4(0)))) {
		out_color = draw_nine_slice();
	} else {
		out_color = draw_texture();
	}
}