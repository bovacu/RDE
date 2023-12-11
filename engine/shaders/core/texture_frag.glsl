#version 330 core

header_2d_frag

void main() {
	// This is just to make the image scale correctly
	vec2 prop = vec2(sub_texture_uv.z - sub_texture_uv.x, sub_texture_uv.w - sub_texture_uv.y);

	// Point of start if sub-texture
	vec2 offset = vec2(sub_texture_uv.x, sub_texture_uv.y);
	vec2 _uv = uv;
    vec2 ratio = (texture_size.xy / size.xy);
    vec2 border = vec2(nine_slice.x, nine_slice.z);
    vec2 bDst = (border / size) * prop;
	vec2 texture_coord = vec2(-1, -1);
	vec2 atlas_offset = vec2(0);

	// Left
    if(_uv.x < offset.x + bDst.x) {
        texture_coord.x = (_uv.x - offset.x) / ratio.x;
		atlas_offset.x = sub_texture_uv.x;
    } 

	// Right
	else if(_uv.x > (sub_texture_uv.z - bDst.x)) {
		texture_coord.x = (_uv.x - sub_texture_uv.z) / ratio.x;
		atlas_offset.x = sub_texture_uv.z;
	}
	
	// Middle
	// else {
	//        float t = (border.x * (texture_size.x - size.x)) / (texture_size.x * (2.0 * border.x - size.x));
	//        texture_coord.x = _uv.x * (1.0 - t * 2.0) + t;
	//    }
	
	// Down
    if(_uv.y < offset.y + bDst.y) {
		texture_coord.y = (_uv.y - offset.y) / ratio.y;
		atlas_offset.y = sub_texture_uv.y;
    }
	
	// Up
	else if(_uv.y > (sub_texture_uv.w - bDst.y)) {
		texture_coord.y = (_uv.y - sub_texture_uv.w) / ratio.y;
		atlas_offset.y = sub_texture_uv.w;
	}

	// Middle
	// else {
	//        float t = (border.y * (texture_size.y - size.y)) / (texture_size.y * (2.0 * border.y - size.y));
	//        texture_coord.y = _uv.y * (1.0 - t * 2.0) + t;
	//    }

	// if(texture_coord.x == -1 || texture_coord.y == -1) {
	// 	out_color = vec4(1, 0, 0, 1);
	// 	return;
	// }

    out_color = texture(tex, texture_coord + atlas_offset);
}