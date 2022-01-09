#version 400 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT{
	vec3 color;
	vec2 dimension;
}gs_in[];

out vec3 fColor;

void build_button(vec4 position){
	fColor = gs_in[0].color;
	gl_Position = position + vec4(-gs_in[0].dimension[0]/2.0, -gs_in[0].dimension[1]/2.0, 0.0, 0.0); // 1:bottom-left   
    EmitVertex();   
    gl_Position = position + vec4( gs_in[0].dimension[0]/2.0, -gs_in[0].dimension[1]/2.0, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-gs_in[0].dimension[0]/2.0,  gs_in[0].dimension[1]/2.0, 0.0, 0.0); // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( gs_in[0].dimension[0]/2.0,  gs_in[0].dimension[1]/2.0, 0.0, 0.0); // 4:top-right
    EmitVertex();
    EndPrimitive();
}

void main(){
	build_button(gl_in[0].gl_Position);
}