#include "map.h"
#include <string>
#include <string.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Lightmap.h"
#include "engine.h"

#include "EntityLight.h"
#include "EntityPhysicsProp.h"
#include "EntityExplosiveBarrel.h"

#include <nlohmann/json.hpp>
#include "Util.h"

Map::Map(std::string fname, Engine* engine)
	:fname(fname), engine(engine)
{
	engine->map = this;
	textureManager = engine->textureManager;
	map = new Mesh(fname.c_str(), textureManager);
	sunVec = -glm::normalize(glm::vec3(0.5, 1, 0.5));
	collisionState = new CollisionState(this);
	collisionState->loadMesh();

	loadEntityState(fname + ".ents");
}

void Map::loadEntityState(std::string fname)
{
	std::ifstream file(fname);
	if (!file)
	{
		*engine->console << "Unable to load entity definition file (" << fname << ")" << std::endl;
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();

	using namespace nlohmann;

	try
	{
		nlohmann::json j = nlohmann::json::parse(buffer.str());

		for (auto& i : j)
		{
			std::string type = i["type"];
			if (i["active"] == 0)
				continue;

			if (type == "EntityLight")
				addLight(new Light(Util::vec3FromString(i["point"]),
						Util::vec3FromString(i["light_col"]),
						i["light_intensity"], 
						i["light_type"],
						Util::vec3FromString(i["light_dir"])
						));
			if (type == "EntityPhysicsProp")
			{
				EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(
					new EntityPhysicsProp(i["modelName"], Util::vec3FromString(i["origin"]))
				);
				e->setTransform(Util::mat4FromString(i["transform"]));
			}
			if (type == "EntityExplosiveBarrel")
			{
				EntityExplosiveBarrel* e = (EntityExplosiveBarrel*)engine->entityManger->addEntity(
					new EntityExplosiveBarrel(Util::vec3FromString(i["origin"]))
				);
				e->setTransform(Util::mat4FromString(i["transform"]));
			}
		}
	}
	catch (json::exception & e)
	{
		*engine->console << "message: " << e.what() << '\n'
			<< "exception id: " << e.id << std::endl;
	}
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