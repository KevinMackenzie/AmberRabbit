#version 120

attribute vec3 _position;

uniform mat4 _mvp;

void main(void)
{
	gl_Position = _mvp * vec4(_position, 1);
}