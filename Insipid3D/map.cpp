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

#include "Util.h"

#include <nlohmann/json.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	skyboxTexture = -1;

	//TODO clean this up, there should be no hardcoded references to textures etc

	float skyboxVertices[] = {      
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	skyboxVAO = vertexArrayObject(skyboxVertices, sizeof(skyboxVertices));
	std::vector<std::string> faces
	{
		"textures/hw_sahara/sahara_rt.tga",
		"textures/hw_sahara/sahara_lf.tga",
		"textures/hw_sahara/sahara_up.tga",
		"textures/hw_sahara/sahara_dn.tga",
		"textures/hw_sahara/sahara_ft.tga",
		"textures/hw_sahara/sahara_bk.tga"
	};
	skyboxTexture = TextureManager::loadCubemap(faces);
}

Map::~Map()
{
	delete collisionState;
	delete map;
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
					new EntityPhysicsProp(i["modelName"], Util::vec3FromString(i["origin"]), i["mass"])
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
	if (skyboxTexture != -1)
	{
		glDepthMask(GL_FALSE);
		GLuint skyboxShader = engine->shaderManager->getShader("shaders/skybox");
		glUseProgram(skyboxShader);
		glActiveTexture(GL_TEXTURE0);
		glm::mat4 clippedView = glm::mat4(glm::mat3(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(clippedView));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		glBindVertexArray(skyboxVAO.vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
	}

	GLuint mapShader = engine->shaderManager->getShader("shaders/map");
	glUseProgram(mapShader);
	glActiveTexture(GL_TEXTURE0);
	glUniformMatrix4fv(glGetUniformLocation(mapShader, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
	glUniformMatrix4fv(glGetUniformLocation(mapShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(mapShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
	glUniform1iARB(glGetUniformLocation(mapShader, "tex"), 0);
	glUniform1iARB(glGetUniformLocation(mapShader, "lightmap"), 1);
	glUniform3fv(glGetUniformLocation(mapShader, "cameraPos"), 1, glm::value_ptr(engine->camera->pos));
	int lI = 0;
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->entParent->active || !lights[i]->worldLight)
		{
			continue;
		}
		std::string lightID = "lights[" + std::to_string(lI) + "]";

		float vis = 0.05;

		glUniform3fv(glGetUniformLocation(mapShader, std::string(lightID + ".pos").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->pos));
		glUniform3fv(glGetUniformLocation(mapShader, std::string(lightID + ".col").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->col));
		glUniform3fv(glGetUniformLocation(mapShader, std::string(lightID + ".dir").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->dir));
		glUniform1f(glGetUniformLocation(mapShader, std::string(lightID + ".intensity").c_str()), lights[i]->intensity * vis);

		lI++;
	}

	glUniform1i(glGetUniformLocation(mapShader, "numLights"), lI);

	map->render();
}

void Map::getLightsShaderUniforms(GLuint shader, glm::vec3 pos)
{
	int lI = 0;
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->entParent->active)
		{
			continue;
		}
		std::string lightID = "lights[" + std::to_string(lI) + "]";

		float vis = 1;

		btVector3 start = Util::vec3Conv(pos);
		btVector3 end = Util::vec3Conv(lights[i]->pos);

		btCollisionWorld::AllHitsRayResultCallback r(start, end);
		collisionState->world->rayTest(start, end, r);

		if (r.hasHit())
		{
			for (int q = 0; q < r.m_collisionObjects.size(); q++)
			{
				//std::cout << r.m_collisionObjects[q]->getCollisionShape()->getName() << std::endl;
				vis = 0;
			}
		}

		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".pos").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->pos));
		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".col").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->col));
		glUniform3fv(glGetUniformLocation(shader, std::string(lightID + ".dir").c_str()), 1, glm::value_ptr(engine->getMap()->lights[i]->dir));
		glUniform1f(glGetUniformLocation(shader, std::string(lightID + ".intensity").c_str()), lights[i]->intensity * vis);

		lI++;
	}

	glUniform1i(glGetUniformLocation(shader, "numLights"), lI);
}

Mesh* Map::getMesh()
{
	return map;
}

EntityLight* Map::addLight(Light* l)
{
	lights.push_back(l);
	l->entParent = (EntityLight*)engine->entityManger->addEntity(new EntityLight(l));
	return l->entParent;
}