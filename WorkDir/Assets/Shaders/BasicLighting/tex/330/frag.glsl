#version 330 core


in VS_OUT
{
	vec3 Position_worldspace;
	vec3 Normal_cameraspace;
	vec3 EyeDirection_cameraspace;
	vec3 LightDirection_cameraspace;
	vec2 uvCoord;
} fs_in;

// Values that stay constant for the whole mesh.
uniform sampler2D m_tex0;
//uniform mat4 MV;
uniform vec3 _li_pos = vec3(4.0f, 4.0f, -4.0f);

out vec4 _Color;

uniform vec3 m_diff;
uniform vec3 m_amb;
uniform vec3 m_spec;
uniform float m_pow;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture2D( m_tex0, fs_in.uvCoord ).rgb;
	//this is if there is no texture;
	if(MaterialDiffuseColor == vec3(0.0f, 0.0f, 0.0f))
	{
		MaterialDiffuseColor = m_diff;
	}
	
	vec3 MaterialAmbientColor = m_amb * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = m_spec;

	// Distance to the light
	float distance = length( _li_pos - fs_in.Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( fs_in.Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( fs_in.LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(fs_in.EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	_Color = vec4(
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,m_pow) / (distance*distance), 1.0);
	
	//_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}