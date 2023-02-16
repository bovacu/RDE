#version 330 core

in vec2 uv;
in vec4 color;
//in vec4 atlas;

uniform sampler2D tex;
out vec4 out_color;

void main(void) {
	float d = texture(tex, uv).r;
	float aaf = fwidth(d);
	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
	out_color = vec4(color.rgb, alpha);
}






//#version 330 core
//
//uniform vec2 atlasResolution;
//uniform float cameraScale;
//
//in vec2 uv;
//in vec4 color;
//in vec4 atlas;
//
//uniform sampler2D tex;
//out vec4 out_color;
//
//float sample(vec2 _uv, float t) {
//	float d = texture(tex, _uv).r;
//	if(d >= t) {
//		return 1.0;
//	}
//	return 0.0;
//}
//
//vec2 atlas_uv(vec2 offset) {
//	vec2 n = vec2(uv.x, 1. - uv.y);
//	vec2 frag_pos = n * atlas.zw + offset / cameraScale;
//	vec2 atlas_pos = atlas.xy + frag_pos;
//	return atlas_pos / atlasResolution;
//}
//
//float f(vec2 offset) {
//	vec2 auv = atlas_uv(offset);
//	return sample(auv, .5);
//}
//
//float sampleSubpixel(vec2 offset) {
//	const int size = 8;
//	float disp = 1.0 / (float(size) + 2.0);
//	float contrb = 0.0;
//	float maxContrb = 0.0;
//
//	for(int j = -size; j <= size; j++) {
//		for(int i = -size; i <= size; i++) {
//			contrb += f(offset + vec2(float(i) * (disp / 3.0), float(j) * disp));
//			maxContrb += 1.0;
//		}
//	}
//
//	return (contrb / maxContrb);
//}
//
//vec4 sampleAntialiasedSubpixel() {
//	float r = sampleSubpixel(-vec2(1.0 / 3.0, 0));
//	float g = sampleSubpixel(vec2(0.));
//	float b = sampleSubpixel( + vec2(1.0 / 3.0, 0));
//
//	vec2 auv = atlas_uv(vec2(0.));
//	float a = sample(auv, .4);
//	vec3 bg = vec3(24.0 / 255.0); // 0x181818FF
//	vec3 rgb = mix(bg, color.rgb, vec3(r, g, b));
//	return vec4(rgb, a);
//}
//
//void main(void) {
//	out_color = sampleAntialiasedSubpixel();
//}