#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 _position;
//attribute vec2 _uv0;
attribute vec3 _normal;

// Output data ; will be interpolated for each fragment.
//varying vec2 UV;
varying vec3 Position_worldspace;
varying vec3 Normal_cameraspace;
varying vec3 EyeDirection_cameraspace;
varying vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 _model;
uniform mat4 _view;
uniform mat4 _mv;
uniform mat4 _mvp;

uniform vec3 _li_pos = vec3(4.0f, 4.0f, -4.0f);

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  _mvp * vec4(_position,1);
	
	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (_model * vec4(_position,1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( _mv * vec4(_position,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( _view * vec4(_li_pos,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( _mv * vec4(_normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// UV of the vertex. No special space for this one.
	//UV = _uv0;
}

