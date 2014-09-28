#version 330 

in vec3 UVW;
uniform samplerCube m_tex0;

out vec4 outColor;
void main(void)
{
	outColor = texture(m_tex0, UVW);
}