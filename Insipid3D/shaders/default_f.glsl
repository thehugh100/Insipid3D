#version 430 core
layout(location = 0) out vec4 FragCol;

struct PointLight
{
	vec3 pos;
	vec3 col;
	vec3 dir;
	float intensity;
};
uniform PointLight lights[32];
uniform int numLights;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec4 sunVec;
uniform vec3 cameraPos;
uniform sampler2D tex;

void main()
{
	vec3 norm = normalize(normal);
	
	float shininess = 0.7;
	vec3 ambient = vec3(0.33, 0.33, 0.33);
	vec3 sunCol = vec3(1., 0.95, 0.9) * 1.1;


	vec3 sunDir   = -sunVec.xyz;
	vec3 viewDir    = normalize(cameraPos - fragPos);
	float diff = max(dot(norm, sunDir), 0.2);
	vec3 diffuse = diff * sunCol * sunVec.w;
	vec3 reflectDir = reflect(-sunDir, norm); 
	vec3 halfwayDir = normalize(sunDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 36.0) * 1.4;
	vec3 specular = sunCol * spec * sunVec.w;

	for(int i = 0; i < numLights; i++)
	{
		vec3 deltaToLight = lights[i].pos - fragPos;
		vec3 dirToLight = normalize(deltaToLight);
		float d = dot(dirToLight, norm);

		if(d < 0.) continue;
		float distToLightSquared = dot(deltaToLight, deltaToLight);
		vec3 reflectDir = reflect(-dirToLight, norm); 
		vec3 halfwayDir = normalize(dirToLight + viewDir);
		float ldiffuse = max(d, 0.f) * (lights[i].intensity * .25);

		diffuse += lights[i].col * (ldiffuse / distToLightSquared);
		specular += lights[i].col * (pow(max(dot(normal, halfwayDir), 0.0), 36.0) * lights[i].intensity) / distToLightSquared;
	}

	vec3 objColor = (texture2D(tex, texCoord).rgb) * (ambient + (diffuse + (specular * shininess)));

    FragCol = vec4(objColor, texture2D(tex, texCoord).a);
} 