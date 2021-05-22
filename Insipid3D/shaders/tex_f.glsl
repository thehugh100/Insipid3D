#version 430 core
layout(location = 0) out vec4 FragCol;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
uniform sampler2D tex;
uniform vec4 col;

void main()
{
    FragCol = texture2D(tex, texCoord).rgba * col;
} 