#version 450
out vec4 FragColor;
in vec2 UV;

uniform float _Time;
uniform float _Speed;

uniform float _Distortion;

uniform sampler2D _Texture;
uniform sampler2D _Noise;

void main()
{
	vec2 noiseVector = vec2(UV.x + (_Time * _Speed), UV.y);
	float noise = texture(_Noise, noiseVector).r;
	vec2 uv = UV + (noise * _Distortion) - (_Distortion / 2);
	FragColor = texture(_Texture, uv);
}