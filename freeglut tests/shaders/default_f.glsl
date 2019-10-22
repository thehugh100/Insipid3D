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
	
	float shininess = 0.55;
	vec3 ambient = vec3(0.2, 0.2, 0.2);
	vec3 sunCol = vec3(1., 0.95, 0.9);



	vec3 sunDir   = normalize(-sunVec.xyz);
	vec3 viewDir    = normalize(cameraPos - fragPos);
	float diff = max(dot(norm, sunDir), 0.0);
	vec3 diffuse = diff * sunCol * sunVec.w;
	vec3 reflectDir = reflect(-sunDir, norm); 
	vec3 halfwayDir = normalize(sunDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = sunCol * spec * sunVec.w;

	for(int i = 0; i < numLights; i++)
	{
		float distToLight = distance(lights[i].pos, fragPos);
		vec3 dirToLight = normalize(lights[i].pos - fragPos);
		float d = dot(dirToLight, norm);

		vec3 reflectDir = reflect(-dirToLight, norm); 
		vec3 halfwayDir = normalize(dirToLight + viewDir);
		float ldiffuse = max(d, 0.f) * (lights[i].intensity * .25);

		diffuse += lights[i].col * (ldiffuse / (distToLight * distToLight));
		specular += lights[i].col * pow(max(dot(normal, halfwayDir), 0.0), 32.0) * lights[i].intensity * .015;
	}

	vec3 objColor = (texture2D(tex, texCoord).rgb) * (ambient + (diffuse + (specular * shininess)));

    FragCol = vec4(objColor, 1.);
} 