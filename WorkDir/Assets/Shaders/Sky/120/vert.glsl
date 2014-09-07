#version 120

attribute vec3 _position;
attribute vec2 _uv0;

varying vec2 UV;

uniform mat4 _mvp;

void main(void)
{
	UV = _uv0;
	
	gl_Position = vec4(_position, 1.0) * _mvp;
}