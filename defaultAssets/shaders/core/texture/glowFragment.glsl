#version 330 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 pixel;

uniform float glow_size = 1;
uniform vec3 glow_colour = vec3(0, 1, 0);
uniform float glow_intensity = 3;
uniform float glow_threshold = 0.25;

void main() {
    pixel = texture(tex, uv);
    if (pixel.a <= glow_threshold) {
        ivec2 size = textureSize(tex, 0);

        float uv_x = uv.x * size.x;
        float uv_y = uv.y * size.y;

        float sum = 0.0;
        for (int n = 0; n < 9; ++n) {
            uv_y = (uv.y * size.y) + (glow_size * float(n - 4.5));
            float h_sum = 0.0;
            h_sum += texelFetch(tex, ivec2(uv_x - (4.0 * glow_size), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - (3.0 * glow_size), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - (2.0 * glow_size), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - glow_size, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + glow_size, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (2.0 * glow_size), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (3.0 * glow_size), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (4.0 * glow_size), uv_y), 0).a;
            sum += h_sum / 9.0;
        }

        pixel = vec4(glow_colour, (sum / 9.0) * glow_intensity);
    }
}