#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

out vec2 UV;
out vec2 characterVec;

uniform float _Time;
uniform float _Speed;

void main(){
	UV = vUV;

	UV = UV * 2.0 - 0.5;

	float x = sin(_Time * _Speed) * 0.5;
	float y = sin(_Time * _Speed) * 0.5;

	characterVec = vec2(UV.x + x, UV.y + y);

	gl_Position = vec4(vPos,1.0);
}