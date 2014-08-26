#version 120 core

attribute vec3 _position;
attribute vec4 _color0;

varying vec4 color;

uniform mat4 _mvp;

void main(void)
{
	gl_Position = _position * _mvp;
	color = _color0;
}