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
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

struct Material{
	float Ka;
	float Kd;
	float Ks;
	float Shininess;
};
uniform Material _Material;

in vec4 LightSpacePos;
uniform sampler2D _ShadowMap;

uniform float _MinBias;
uniform float _MaxBias;

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos, float bias)
{
	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;
	
	float myDepth = sampleCoord.z - bias;
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;

	float shadow = 0;
	vec2 texelOffset = 1.0 / textureSize(_ShadowMap,0);
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			vec2 uv = sampleCoord.xy + vec2(x * texelOffset.x, y * texelOffset.y);
			shadow += step(texture(_ShadowMap, uv).r, myDepth);
		}
	}
	shadow /= 9.0;

	return shadow;
	//return step(shadowMapDepth, myDepth);
}

void main()
{
	// normal
	vec3 normal = normalize(fs_in.WorldNormal);

	// lighting
	// light pointing down
	vec3 toLight = -_LightDirection;

	float diffuseFactor = max(dot(normal, toLight), 0.0);
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);


	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	// shadows
	float bias = max(_MaxBias * (1.0 - dot(normal,toLight)),_MinBias);

	float shadow = calcShadow(_ShadowMap, LightSpacePos, bias);

	//vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor *= (1.0 - shadow);
	lightColor += _AmbientColor * _Material.Ka;
	//vec3 lightColor = ((_AmbientColor * _Material.Ka) + (1.0 - shadow) * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor)) * objectColor;
	

	FragColor = vec4(objectColor * lightColor, 1.0);
}



