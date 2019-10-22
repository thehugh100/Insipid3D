#pragma once
#include "Mesh.h"
#include "TextureManager.h"
#include "Light.h"

#include "CollisionState.h"

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

struct Map
{
	Map(std::string fname, TextureManager* textureManager);
	void generateLightmap();
	void render();
	Mesh* getMesh();
	Mesh* map;
	TextureManager* textureManager;
	std::vector<Light> lights;
	std::string fname;

	CollisionState* collisionState;

	glm::vec3 sunVec;
};