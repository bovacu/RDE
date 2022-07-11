#version 330 core

uniform vec4 params;
uniform vec4 color;
out vec4 out_color;

void main (void) {
    vec2 position = ( gl_FragCoord.xy / params.xy );
    int _isLine = int(ceil(params.z));

    int _x = int(floor(position.x * params.x));
    float _yOffset = ceil(params.y / params.w) - (params.y / params.w);
    float _xOffset = ceil(params.x / params.z) - (params.x / params.z);
    int _y = int(floor(position.y * params.y));

    if(_x % int(params.x / 2) == 0 || _y % int(params.y / 2) == 0) {
        out_color = vec4(0, 0, 1, 1);
    }

//    else if(int(position.x * params.x) % _isLine == 0 || int(position.y * params.y + _yOffset * params.y / 2.0) % _isLine == 0) {
//        out_color = color;
//    }

    else
        out_color = vec4(0, 0, 0, 0);
}