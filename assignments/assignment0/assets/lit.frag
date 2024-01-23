#version 450
out vec4 FragColor; //The color of this fragment
in Surface
{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;
uniform sampler2D _MainTex;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);

void main()
{
	// make sure fragment normal is still length 1 after interpolation
	vec3 normal = normalize(fs_in.WorldNormal);
	// light pointing down
	vec3 toLight = -_LightDirection;

	float diffuseFactor = max(dot(normal, toLight), 0.0);
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), 128);

	vec3 lightColor = (diffuseFactor + specularFactor) * _LightColor;
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}
