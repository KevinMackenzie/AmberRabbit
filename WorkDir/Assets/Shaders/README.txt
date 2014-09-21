-uniforms have the following requried names:
	-_model
	-_view
	-_mv
	-_mvp
	
-Attributes have the following required names
	-_position
	-_color0-_color7
	-_normal
	-_uv0-_uv7
	
//TODO: how does emissive work
-material is as follows
	-m_diff;
	-m_amb;
	-m_spec;
	-m_pow;
	-m_tex0-mat_tex7
	
//the shader loading system chooses the maximum support version of the shader

//In the .prog file, the GLSL version attributes must be in ascending order