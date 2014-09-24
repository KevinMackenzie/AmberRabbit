#version 120

uniform vec3 m_diff = vec3(150, 150, 150);

void main(void)
{
	gl_FragColor = vec4(m_diff, 1.0);
}