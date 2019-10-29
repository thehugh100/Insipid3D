#pragma once
#include <glm/glm.hpp>

#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_SUN 2

struct EntityLight;

struct Light
{
	Light(glm::vec3 pos, glm::vec3 col, float intensity, uint8_t type = LIGHT_POINT, glm::vec3 dir = glm::vec3(0, -1, 0));


	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 dir;
	float intensity;
	uint8_t type;
	EntityLight* entParent;
};