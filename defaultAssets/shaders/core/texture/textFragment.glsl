#version 330 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 out_color;

void main(void) {
	float d = texture(tex, uv).r;
	float aaf = fwidth(d);
	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
	out_color = vec4(color.rgb, alpha);
}