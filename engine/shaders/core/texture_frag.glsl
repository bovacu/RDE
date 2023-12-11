#version 330 core

header_2d_frag

void main() {
	// This is just to make the image scale correctly
	vec2 prop = vec2(sub_texture_uv.z - sub_texture_uv.x, sub_texture_uv.w - sub_texture_uv.y);

	// Point of start if sub-texture
	vec2 offset = vec2(sub_texture_uv.x, sub_texture_uv.y);
	vec2 _uv = uv;
    vec2 ratio = (texture_size.xy / size.xy) * prop;
    vec2 border = vec2(nine_slice.x, nine_slice.z);
    vec2 bSrc = border / texture_size;
    vec2 bDst = border / size;

	// Left
    if(_uv.x < offset.x + bDst.x) {
        _uv.x = (_uv.x + offset.x) / ratio.x;
    } 

	// Right
	else if(_uv.x > (sub_texture_uv.z - bDst.x)) {
        _uv.x = 1.0 - (sub_texture_uv.z - _uv.x) / ratio.x;
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
	else if(_uv.y > (sub_texture_uv.w - bDst.y)) {
        _uv.y = 1.0 - (sub_texture_uv.w - _uv.y) / ratio.y;
    } 

	// Middle
	else {
        float t = (border.y * (texture_size.y - size.y)) / (texture_size.y * (2.0 * border.y - size.y));
        _uv.y = _uv.y * (1.0 - t * 2.0) + t;
    }

    out_color = texture(tex, (prop * _uv + offset));
}