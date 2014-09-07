#version 330 

in vec2 UV;
uniform sampler2D mat_tex0;

out vec4 outColor;
void main(void)
{
	outColor = texture2D(mat_tex0, UV);
}