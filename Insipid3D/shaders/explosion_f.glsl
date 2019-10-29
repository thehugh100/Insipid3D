#version 430 core
layout(location = 0) out vec4 FragCol;

uniform sampler2D tex;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec3 di;

uniform float scale;

void main()
{
	vec3 col = vec3(0.);
	col.x = 1.2-di.x * .9;
    col.y = 1.-di.x * 1.3;
    col.z = 1.-di.x * 2.5;

	float alpha = 1.-pow(1.-scale, 2);

    FragCol = vec4(col, alpha);
} 