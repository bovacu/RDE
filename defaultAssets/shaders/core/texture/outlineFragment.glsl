#version 330 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;
out vec4 pixel;
uniform float outline_thickness = .2;
uniform vec3 outline_colour = vec3(0, 0, 1);
uniform float outline_threshold = .5;

void main(void) {
    pixel = texture(tex, uv);

    if (pixel.a <= outline_threshold) {
        ivec2 size = textureSize(tex, 0);

        float uv_x = uv.x * size.x;
        float uv_y = uv.y * size.y;

        float sum = 0.0;
        for (int n = 0; n < 9; ++n) {
            uv_y = (uv.y * size.y) + (outline_thickness * float(n - 4.5));
            float h_sum = 0.0;
            h_sum += texelFetch(tex, ivec2(uv_x - (4.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x - outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(tex, ivec2(uv_x + (4.0 * outline_thickness), uv_y), 0).a;
            sum += h_sum / 9.0;
        }

        if (sum / 9.0 >= 0.0001) {
            pixel = vec4(outline_colour, 1);
        }
    }
}
