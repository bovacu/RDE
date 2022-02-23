#version 400 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 pixel;

uniform vec2 blur_size = vec2(.5, .5);

void main() {
    ivec2 size = textureSize(tex, 0);

    float uv_x = uv.x * size.x;
    float uv_y = uv.y * size.y;

    vec4 sum = vec4(0.0);
    for (int n = 0; n < 9; ++n) {
        uv_y = (uv.y * size.y) + (blur_size.y * float(n - 4.5));
        vec4 h_sum = vec4(0.0);
        h_sum += texelFetch(tex, ivec2(uv_x - (4.0 * blur_size.x), uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x - (3.0 * blur_size.x), uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x - (2.0 * blur_size.x), uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x - blur_size.x, uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x, uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x + blur_size.x, uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x + (2.0 * blur_size.x), uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x + (3.0 * blur_size.x), uv_y), 0);
        h_sum += texelFetch(tex, ivec2(uv_x + (4.0 * blur_size.x), uv_y), 0);
        sum += h_sum / 9.0;
    }

    pixel = sum / 9.0;
}