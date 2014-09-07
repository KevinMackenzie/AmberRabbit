#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 _position;

uniform mat4 _mvp;

void main(void)
{
	gl_Position = vec4(_position, 1.0) * _mvp;
}