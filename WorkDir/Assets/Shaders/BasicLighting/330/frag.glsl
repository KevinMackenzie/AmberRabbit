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
uniform sampler2D mat_tex0;
//uniform mat4 MV;
uniform vec3 _light_position;

uniform vec4 mat_diffuse;
uniform vec4 mat_ambient;
uniform vec4 mat_specular;
uniform float mat_power;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture2D( mat_tex0, fs_in.uvCoord ).rgb;
	//this is if there is no texture;
	if(MaterialDiffuseColor.a == 0.0f)
	{
		MaterialDiffuseColor = mat_diffuse;
	}
	
	vec3 MaterialAmbientColor = mat_ambient * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = mat_specular;

	// Distance to the light
	float distance = length( LightPosition_worldspace - fs_in.Position_worldspace );

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
	
	gl_FragColor = vec4(
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,mat_power) / (distance*distance), 1.0);
	
	//_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}