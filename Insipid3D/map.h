#pragma once
#include "Mesh.h"
#include "TextureManager.h"
#include "Light.h"
#include "CollisionState.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "VAO.h"

struct Engine;

struct Map
{
	Map(std::string fname, Engine* engine);
	~Map();

	void generateLightmap();
	void render();
	void getLightsShaderUniforms(GLuint shader, glm::vec3 pos);
	Mesh* getMesh();
	EntityLight* addLight(Light* l);
	void loadEntityState(std::string fname);

	Mesh* map;
	TextureManager* textureManager;
	std::vector<Light *> lights;
	std::string fname;
	CollisionState* collisionState;
	Engine* engine;
	glm::vec3 sunVec;
	GLuint skyboxTexture;
	vertexArrayObject skyboxVAO;
};