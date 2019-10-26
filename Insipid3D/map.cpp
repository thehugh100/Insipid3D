#include "map.h"
#include <string>
#include <string.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Lightmap.h"
#include "engine.h"
#include "EntityLight.h"

Map::Map(std::string fname, Engine* engine)
	:fname(fname), engine(engine)
{
	textureManager = engine->textureManager;
	map = new Mesh(fname.c_str(), textureManager);
	sunVec = -glm::normalize(glm::vec3(0.5, 1, 0.5));
	collisionState = new CollisionState(this);
	collisionState->loadMesh();
}

void Map::generateLightmap()
{
	std::string lightmapName = fname + ".png";
	if (std::filesystem::exists(lightmapName))
	{
		map->hasLightmap = 1;
		map->lightmap = textureManager->loadTexture(lightmapName.c_str(), 1);
	}
	else
	{
		LightmapGenerator* l = new LightmapGenerator();
		l->lightmapCalc(this, 2048, lights, textureManager);
	}
}

void Map::render()
{
	/*
	glUseProgram(flatShader);
	glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniform3f(glGetUniformLocation(flatShader, "col"), 0., 0., 0.);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	concrete_floor->render();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_DEPTH_BUFFER_BIT);
	*/

	map->render();
}

Mesh* Map::getMesh()
{
	return map;
}

void Map::addLight(Light* l)
{
	lights.push_back(l);
	engine->entityManger->addEntity(new EntityLight(l));
}
