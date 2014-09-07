#version 120

attribute vec3 _position;
attribute vec4 _color0;

varying vec4 color;

uniform mat4 _mvp;

void main(void)
{
	gl_Position = vec4(_position, 1.0f) * _mvp;
	color = _color0;
}