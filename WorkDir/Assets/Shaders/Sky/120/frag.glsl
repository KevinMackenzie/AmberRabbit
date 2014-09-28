#version 120 
#extension GL_NV_shadow_samplers_cube : enable 

varying vec3 UVW;
uniform samplerCube m_tex0;

void main(void)
{
	gl_FragColor = textureCube(m_tex0, UVW);
}