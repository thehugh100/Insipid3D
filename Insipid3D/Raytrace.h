#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include "Mesh.h"

struct rayHit
{
	bool hit;
	glm::vec3 pos;
	glm::vec3 normal;
	float distance;
};

struct RayTrace
{
	static inline int rayIntersectsTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float* intersection);
	static rayHit rayTrace(glm::vec3 origin, glm::vec3 rayDir, Mesh* mesh);
};