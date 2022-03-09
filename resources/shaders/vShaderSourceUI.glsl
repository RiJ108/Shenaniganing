#version 400 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aDimension;

out VS_OUT{
	vec3 color;
	vec2 dimension;
}vs_out;

void main(){
	gl_Position = vec4(aPosition, 0.0, 1.0);
	vs_out.color = aColor;
	vs_out.dimension = aDimension;
}