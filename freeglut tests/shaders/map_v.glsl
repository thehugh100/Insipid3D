#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 ind;
layout (location = 4) in vec2 aLightMapCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out vec2 lightMapCoord;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1);
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = aNormal;
	texCoord = aTexCoord;
	lightMapCoord = aLightMapCoord;
	lightMapCoord.y = lightMapCoord.y;
}