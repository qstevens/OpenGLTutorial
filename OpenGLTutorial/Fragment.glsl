#version 330 core

in vec2 UV;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 Position_worldspace;

layout(location = 0) out vec3 color;

uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 50.0f;

	vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
	float distance = length(LightPosition_worldspace - Position_worldspace);

	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);

	float cosTheta = clamp(dot(n,l), 0, 1);

	vec3 Eye = normalize(EyeDirection_cameraspace);
	vec3 Reflect = reflect(-l,n);
	
	float cosAlpha = clamp(dot(Eye, Reflect), 0, 1);

	color.rgb = MaterialAmbientColor + 
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) + 
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5)/(distance*distance);
}