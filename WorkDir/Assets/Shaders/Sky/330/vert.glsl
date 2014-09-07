#version 330

in vec3 _position;
in vec2 _uv0;

out vec2 UV;

uniform mat4 _mvp;

void main(void)
{
	UV = _uv0;
	
	gl_Position = vec4(_position, 1.0) * _mvp;
}