#version 120

attribute vec3 _position;

varying vec3 UVW;

uniform mat4 _mvp;

void main(void)
{
	UVW = _position;
	
	gl_Position = _mvp * vec4(_position, 1.0);
}