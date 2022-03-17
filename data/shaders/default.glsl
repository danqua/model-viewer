#vertex
#version 450 core
layout (location = 0) in vec3 VertPosition;
layout (location = 1) in vec3 VertNormal;
layout (location = 2) in vec2 VertTexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 LightSpaceMatrix;

out VS_OUT {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec3 CameraPosition;
	vec4 PositionLightSpace;
} Out;

void main()
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(VertPosition, 1.0);
	Out.Position = vec3(ModelMatrix * vec4(VertPosition, 1.0));
	Out.Normal = mat3(transpose(inverse(ModelMatrix))) * VertNormal;
	Out.TexCoord = VertTexCoord;
	Out.CameraPosition = inverse(ViewMatrix)[3].xyz;
	Out.PositionLightSpace = LightSpaceMatrix * vec4(Out.Position, 1.0);
}

#fragment
#version 450 core

#define MAX_POINT_LIGHTS 8

out vec4 FragColor;

struct light
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct directional_light
{
	light Light;
	vec3 Direction;
};

struct point_light
{
	light Light;
	vec3 Position;
	float Constant;
	float Linear;
	float Quadratic;
};

struct material
{
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Shininess;
};

uniform directional_light DirectionalLight;

uniform float FarPlane;
uniform int NumPointLights;
uniform point_light PointLights[MAX_POINT_LIGHTS];

uniform material Material;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D DirectionLightShadowMap;
//uniform samplerCube EnvironmentMap;
uniform samplerCube DepthMap;


in VS_OUT {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec3 CameraPosition;
	vec4 PositionLightSpace;
} In;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0, 1] range
	projCoords = projCoords * 0.5 + 0.5;
	// Get closest depth value from light's perspective
	float closestDepth = texture(DirectionLightShadowMap, projCoords.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// Little bias to solve shadow acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
	// Check whether current frag pos is in shadow
	//float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	//return shadow;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(DirectionLightShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(DirectionLightShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}


float CubeShadowCalc(vec3 fragPos, point_light PointLight)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - PointLight.Position;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(DepthMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= FarPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}  

vec4 CalcDirectionalLight(directional_light DirectionalLight, vec3 Normal, vec3 ViewDirection)
{
	vec3 LightDirection = normalize(-DirectionalLight.Direction);

	float shadow = ShadowCalculation(In.PositionLightSpace, Normal, LightDirection);

	// Diffuse shading
	float DiffuseFactor = max(dot(Normal, LightDirection), 0.0);
	// Specular shading
	vec3 ReflectDir = reflect(-LightDirection, Normal);
	
	float SpecularFactor = pow(max(dot(ViewDirection, ReflectDir), 0.0), Material.Shininess);

	
	//float ratio = 1.00 / 1.52;
	//vec3 I = normalize(In.Position - In.CameraPosition);
	//vec3 R = refract(I, -normalize(Normal), ratio);
	//vec4 EnvMap = texture(EnvironmentMap, R);

	// combine results
	vec4 AmbientColor =	vec4(DirectionalLight.Light.Color, 1.0) * 
						texture(DiffuseTexture, In.TexCoord) *
						DirectionalLight.Light.AmbientIntensity *
						vec4(Material.AmbientColor, 1.0);
	vec4 DiffuseColor =	vec4(DirectionalLight.Light.Color, 1.0) * 
						texture(DiffuseTexture, In.TexCoord) *
						DirectionalLight.Light.DiffuseIntensity *
						vec4(Material.DiffuseColor, 1.0) *
						DiffuseFactor;
	vec4 SpecularColor = vec4(DirectionalLight.Light.Color, 1.0) *
						 vec4(Material.SpecularColor, 1.0) *
						 texture(SpecularTexture, In.TexCoord) *
						 SpecularFactor;


	return (AmbientColor + ((1.0 - shadow) * (DiffuseColor + SpecularColor)));
	
}

vec4 CalcPointLight(point_light PointLight, vec3 Normal, vec3 FragPos, vec3 ViewDirection)
{
	vec3 LightDirection = normalize(PointLight.Position - FragPos);
	// Diffuse shading
	float DiffuseFactor = max(dot(Normal, LightDirection), 0.0);

	float shadow = CubeShadowCalc(FragPos, PointLight);


	// Specular shading
	vec3 ReflectDir = reflect(-LightDirection, Normal);
	float SpecularFactor = pow(max(dot(ViewDirection, ReflectDir), 0.0), Material.Shininess);
	// Attenuation
	float Distance = length(PointLight.Position - FragPos);
	float Attenuation = 1.0 / (
		PointLight.Constant +
		PointLight.Linear * Distance +
		PointLight.Quadratic * (Distance * Distance)
	);
	// Combine Results
	vec4 AmbientColor =	vec4(PointLight.Light.Color, 1.0) * 
						texture(DiffuseTexture, In.TexCoord) *
						PointLight.Light.AmbientIntensity *
						vec4(Material.AmbientColor, 1.0);
	vec4 DiffuseColor =	vec4(PointLight.Light.Color, 1.0) * 
						texture(DiffuseTexture, In.TexCoord) *
						PointLight.Light.DiffuseIntensity *
						vec4(Material.DiffuseColor, 1.0) *
						DiffuseFactor;
	vec4 SpecularColor = vec4(PointLight.Light.Color, 1.0) *
						 vec4(Material.SpecularColor, 1.0) *
						 texture(SpecularTexture, In.TexCoord) *
						 SpecularFactor;

	return (AmbientColor + ((1.0 - shadow) * (DiffuseColor + SpecularColor))) * Attenuation;
	
	//return (AmbientColor + DiffuseColor + SpecularColor) * Attenuation;
}

void main()
{
	float Gamma = 2.2;
	vec3 ViewDir = normalize(In.CameraPosition - In.Position);
	vec3 Normal = normalize(In.Normal);
	FragColor = CalcDirectionalLight(DirectionalLight, Normal, ViewDir);

	for (int i = 0; i < NumPointLights; i++)
	{
		FragColor += CalcPointLight(PointLights[i], Normal, In.Position, ViewDir);
	}

	FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / Gamma));


	/*

    vec3 fragToLight = In.Position - PointLights[0].Position;
    float closestDepth = texture(DepthMap, fragToLight).r;
    closestDepth *= FarPlane;

	FragColor = vec4(vec3(closestDepth / FarPlane), 1.0);
	*/
}