#version 330 core

header_2d_frag

void main(void) {
	float d = texture(tex, uv).r;
	float aaf = fwidth(d);
	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
	out_color = vec4(color.rgb, alpha);
}