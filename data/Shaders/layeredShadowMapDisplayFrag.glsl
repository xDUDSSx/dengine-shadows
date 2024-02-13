#version 330 core

layout (location = 0) out vec4 FragColor;

#define PSSM_CASCADES 4

uniform sampler2DArray sourceLayered;
uniform sampler2D source;

uniform vec2 u_resolution;
uniform bool u_layered;

float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;

	const float step = 0.1;
	float aspect = u_resolution.y / u_resolution.x;

	if (u_layered) {
		for (int i = 1; i < PSSM_CASCADES + 1; i++) {
			if (uv.x < step * i && uv.y * aspect < step) {
				float cX = map(uv.x, step * (i-1), step * i, 0., 1.);
				float cY = map(uv.y, 0., step, 0., 1.);
				FragColor = vec4(vec3(texture(sourceLayered, vec3(vec2(cX, cY), i-1)).r), 1.0);
				return;
			}
		}
	} else {
		if (uv.x < step && uv.y * aspect < step) {
			float cX = map(uv.x, 0, step, 0., 1.);
			float cY = map(uv.y, 0, step, 0., 1.);
			FragColor = vec4(vec3(texture(source, vec2(cX, cY)).r), 1.0);
			return;
		}
	}

	discard;
}