#version 330

in vec3 _position;

out vec3 UVW;

//make this non-standard, because the sky is special
uniform mat4 _mvp;

void main(void)
{
	UVW = _position;
	
	gl_Position =  _mvp * vec4(_position, 1.0);
}