#version 330 core

in vec2 uv;
in vec2 position;
in vec4 color;

uniform vec4 backgroundColor;
uniform float zoom;
uniform vec2 resolution;
uniform vec2 cameraDisplacement;
uniform sampler2D tex;
out vec4 out_color;

void main() {
	vec2 coord = gl_FragCoord.xy;
	int baseSize = 128;
	float zoomInverse = 1.0 / zoom;
	int gridSize = int(baseSize * zoomInverse);
	float yDisplacement = gridSize * (baseSize / resolution.y) + 1;

	float halfHeight = resolution.y / 2.0;
	float halfWidth = resolution.x / 2.0;
	
	if(coord.y >= (-cameraDisplacement.y * zoomInverse + halfHeight) - 1 && coord.y <= (-cameraDisplacement.y * zoomInverse + halfHeight) + 1 || 
		coord.x >= (-cameraDisplacement.x * zoomInverse + halfWidth) - 1 && coord.x <= (-cameraDisplacement.x * zoomInverse + halfWidth) + 1) {
		out_color = vec4(63.0 / 255.0, 63.0 / 255.0, 63.0 / 255.0, 1.0);
	} else if(int(coord.y + yDisplacement + cameraDisplacement.y) % gridSize == 0 || int(coord.x + cameraDisplacement.x * zoomInverse) % gridSize == 0) {
		out_color = vec4(200.0 / 255.0, 200.0 / 255.0, 200.0 / 255.0, 0.75);
	} else {
		out_color = backgroundColor;
	}
}