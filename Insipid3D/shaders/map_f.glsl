#version 430 core
layout(location = 0) out vec4 FragCol;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec2 lightMapCoord;

struct PointLight
{
	vec3 pos;
	vec3 col;
	vec3 dir;
	float intensity;
};
uniform PointLight lights[32];
uniform int numLights;

uniform sampler2D tex;
uniform vec3 cameraPos;
uniform sampler2D lightmap;

void main()
{
	vec3 oCol = texture2D(tex, texCoord).rgb;
	vec3 lightCol = 0.15 + texture2D(lightmap, lightMapCoord).rgb;

	vec3 viewDir    = normalize(cameraPos - fragPos);

	for(int i = 0; i < numLights; i++)
	{
		vec3 deltaToLight = lights[i].pos - fragPos;
		vec3 dirToLight = normalize(deltaToLight);
		float d = dot(dirToLight, normal);

		if(d < 0) continue;
		float distToLightSquared = dot(deltaToLight, deltaToLight);
		vec3 reflectDir = reflect(-dirToLight, normal); 
		vec3 halfwayDir = normalize(dirToLight + viewDir);
		float ldiffuse = max(d, 0.f) * (lights[i].intensity * .45);

		oCol += lights[i].col * (ldiffuse / distToLightSquared);
		oCol += lights[i].col * (pow(max(dot(normal, halfwayDir), 0.0), 32.0) * lights[i].intensity) / distToLightSquared;
	}

    FragCol = vec4(oCol * lightCol, 1.);
} 