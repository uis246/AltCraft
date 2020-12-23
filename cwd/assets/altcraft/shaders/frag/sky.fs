#version 330 core
#extension ARB_shader_image_load_store: enable

#ifdef GL_ARB_shader_image_load_store
layout(early_fragment_tests) in;
#endif

in vec2 uvPos;
in vec3 pos;

uniform sampler2DArray textureAtlas;
uniform float DayTime;
uniform vec4 sunTexture;
uniform float sunTextureLayer;
uniform vec4 moonTexture;
uniform float moonTextureLayer;

const vec4 DaySkyColor = vec4(0.49,0.66,1, 1);

const vec3 SunPos = vec3(0,0.1,0.5);

const vec3 MoonPos = vec3(0,0.1,-0.5);

vec3 TransformTextureCoord(vec4 TextureAtlasCoords, vec2 UvCoords, float Layer) {
    vec2 transformed = TextureAtlasCoords.xy + UvCoords * TextureAtlasCoords.zw;
    return vec3(transformed, Layer);
}

vec4 Sun() {
    vec3 sunDelta = (pos - SunPos)*3.0f;
    float distanceToSun = length(sunDelta);
    vec4 sunColor = texture(textureAtlas, TransformTextureCoord(sunTexture,(sunDelta.xy+0.5f), sunTextureLayer));
    vec4 sun = mix(vec4(0,0,0,1),sunColor,clamp(1-distanceToSun*2.0f,0,1));
    return sun;
}

vec4 Moon() {
    vec3 moonDelta = (pos - MoonPos)*4.5f;
    float distanceToMoon = length(moonDelta);
    vec4 moonColor = texture(textureAtlas, TransformTextureCoord(moonTexture, (moonDelta.xy+0.5f), moonTextureLayer));
    vec4 moon = mix(vec4(0,0,0,1),moonColor,clamp(1-distanceToMoon*2.0f,0,1));
    return moon;
}

void main() {
    vec4 starColor = vec4(0.0f, 0.04f, 0.06f, 1.0f);
    gl_FragColor = mix(starColor, DaySkyColor, DayTime);
    gl_FragColor += Sun();
    gl_FragColor += Moon();
}
