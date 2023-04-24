#version 330 core

in vec2 uv;
in vec2 position;
in vec4 color;

uniform vec4 gridColor;
uniform float zoom;
uniform vec2 resolution;
uniform vec2 cameraDisplacement;
uniform sampler2D tex;
out vec4 out_color;

void main() {
	//int baseSize = 128;
	//int gridSize = int(ceil(baseSize * zoomInverse));
	//int yDisplacement = int(resolution.y) % gridSize;
	
	vec2 coord = gl_FragCoord.xy;
	float zoomInverse = 1.0 / zoom;
	float halfHeight = resolution.y / 2.0;
	float halfWidth = resolution.x / 2.0;
	
	if(coord.y >= (-cameraDisplacement.y * zoomInverse + halfHeight) - 1 && coord.y <= (-cameraDisplacement.y * zoomInverse + halfHeight) + 1 || 
		coord.x >= (-cameraDisplacement.x * zoomInverse + halfWidth) - 1 && coord.x <= (-cameraDisplacement.x * zoomInverse + halfWidth) + 1) {
		out_color = gridColor;
	} 
	//else if(int(ceil(coord.x - yDisplacement * gridSize + cameraDisplacement.x * zoomInverse)) % gridSize == 0) {
	//	out_color = vec4(200.0 / 255.0, 200.0 / 255.0, 200.0 / 255.0, 0.75);
	//}
	else {
		discard;
	}
}