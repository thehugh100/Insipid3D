#pragma once
#include "Mesh.h"
#include "TextureManager.h"
#include "Light.h"
#include "CollisionState.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

struct Engine;

struct Map
{
	Map(std::string fname, Engine* engine);
	void generateLightmap();
	void render();
	Mesh* getMesh();
	void addLight(Light* l);
	void loadEntityState(std::string fname);

	Mesh* map;
	TextureManager* textureManager;
	std::vector<Light *> lights;
	std::string fname;
	CollisionState* collisionState;
	Engine* engine;
	glm::vec3 sunVec;
};