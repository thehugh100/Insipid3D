#version 430 core
layout(location = 0) out vec4 FragCol;

in vec3 normal;
in vec3 fragPos;
uniform in vec3 viewPos;

uniform samplerCube skybox;

void main()
{
	vec3 lightColor = vec3(1., 1., 1.);
	vec3 objectColor = vec3(0.3, 0.3, 0.3);
	float shininess = 0.7;

	vec3 lightPos = vec3(-5,10,0); 
	vec3 norm = normalize(normal);

	vec3 lightDir   = normalize(lightPos - fragPos);
	vec3 viewDir    = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5;

	vec3 reflectDir = reflect(-lightDir, norm); 

	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = lightColor * spec;

	vec3 result = (ambient + diffuse) * objectColor;

    FragCol = vec4(result, 1.);
} 