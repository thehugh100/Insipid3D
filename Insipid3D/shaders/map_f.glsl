#version 430 core
layout(location = 0) out vec4 FragCol;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec2 lightMapCoord;

uniform sampler2D tex;
uniform sampler2D lightmap;

void main()
{
	vec3 oCol = texture2D(tex, texCoord).rgb;
	vec3 lightCol = 0.15 + texture2D(lightmap, lightMapCoord).rgb;
    FragCol = vec4(oCol * lightCol, 1.);
} 