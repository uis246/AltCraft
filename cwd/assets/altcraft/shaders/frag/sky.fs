#version 330 core

in vec2 uvPos;
in vec3 pos;

uniform sampler2DArray textureAtlas;
uniform float DayTime;

//xy - sun
//zw - moon
uniform vec4 texturePositions;
uniform vec4 textureSizes;

uniform uint sunTextureLayer;
uniform uint moonTextureLayer;

const vec4 DaySkyColor = vec4(0.49,0.66,1, 1);

const vec3 SunPos = vec3(0,0.1,0.5);

const vec3 MoonPos = vec3(0,0.1,-0.5);


void main() {
	vec4 outputColor = mix(vec4(0.0f, 0.04f, 0.06f, 1.0f), DaySkyColor, DayTime);

	vec3 sunDelta = (pos - SunPos)*3.0f;
	float distanceToSun = length(sunDelta);

	vec3 moonDelta = (pos - MoonPos)*4.5f;
	float distanceToMoon = length(moonDelta);

	vec4 transformed = texturePositions + textureSizes * (vec4(sunDelta.xy, moonDelta.xy) + 0.5);

	vec4 sunColor = texture(textureAtlas, vec3(transformed.xy, sunTextureLayer));
	vec4 sun = mix(vec4(0,0,0,1),sunColor,clamp(1-distanceToSun*2.0f,0,1));
	outputColor += sun;

	vec4 moonColor = texture(textureAtlas, vec3(transformed.zw, moonTextureLayer));
	vec4 moon = mix(vec4(0,0,0,1),moonColor,clamp(1-distanceToMoon*2.0f,0,1));
	outputColor += moon;

	gl_FragColor = outputColor;
}
