#version 120 

varying vec2 UV;
uniform sampler2D mat_tex0;

void main(void)
{
	gl_FragColor = texture2D(mat_tex0, UV);
}