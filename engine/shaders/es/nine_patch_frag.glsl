#version 300 es
precision mediump float;

header_2d_frag

void main() {
    	vec2 _prop = vec2(sub_texture_uv.z - sub_texture_uv.x, sub_texture_uv.w - sub_texture_uv.y);

	// Point of start if sub-texture
    vec2 _ratio = (texture_size.xy / size.xy);
    vec2 _border = vec2(nine_slice.x, nine_slice.z);
    vec2 _border_custom_prop = (_border / size) * _prop;
    vec2 _border_original_prop = (_border / texture_size) * _prop;
	vec2 _texture_coord = vec2(0);
	vec2 _atlas_offset = vec2(0);

	//		x padding min	x padding max
	//       	   |			|
	// 	  *------*------------*------*
	// 	  |	  |			|	  |
	// 	  |  L/T |	 M/T	|  R/T |
	// 	  |	  |			|	  |
	// 	--*------*------------*------*-- y padding max
	// 	  |	  |			|	  |
	// 	  |  L/M |	 M/M	|  R/M |
	// 	  |	  |			|	  |
	// 	--*------*------------*------*-- y padding min
	// 	  |	  |			|	  |
	// 	  |  L/D |	 M/D	|  R/D |
	// 	  |	  |			|	  |
	// 	  *------*------------*------*

	// --------------------- X Axis
	// Left
    if(uv.x < sub_texture_uv.x + _border_custom_prop.x) {
        _texture_coord.x = (uv.x - sub_texture_uv.x) / _ratio.x;
		_atlas_offset.x = sub_texture_uv.x;
    }

	// Right
	else if(uv.x > (sub_texture_uv.z - _border_custom_prop.x)) {
		_texture_coord.x = (uv.x - sub_texture_uv.z) / _ratio.x;
		_atlas_offset.x = sub_texture_uv.z;
	}

	// Middle
	else {
		_texture_coord.x = uv.x + (_border_original_prop.x - _border_custom_prop.x);
		if(_texture_coord.x > sub_texture_uv.z - _border_original_prop.x) {
			_texture_coord.x = sub_texture_uv.z - _border_original_prop.x;
		}
	}

	// --------------------- Y Axis
	// Down
    if(uv.y < sub_texture_uv.y + _border_custom_prop.y) {
		_texture_coord.y = (uv.y - sub_texture_uv.y) / _ratio.y;
		_atlas_offset.y = sub_texture_uv.y;
    }

	// Up
	else if(uv.y > (sub_texture_uv.w - _border_custom_prop.y)) {
		_texture_coord.y = (uv.y - sub_texture_uv.w) / _ratio.y;
		_atlas_offset.y = sub_texture_uv.w;
	}

	// Middle
	else {
		_texture_coord.y = uv.y + _border_original_prop.y - _border_custom_prop.y;
		if(_texture_coord.y > sub_texture_uv.w - _border_original_prop.y) {
			_texture_coord.y = sub_texture_uv.w - _border_original_prop.y;
		}
	}

	vec4 _color = texture(tex, _texture_coord + _atlas_offset);
	if(_color.w < 0.05) {
		discard;
	}

    out_color = texture(tex, _texture_coord + _atlas_offset) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}
