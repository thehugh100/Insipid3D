#pragma once

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

struct Engine;
struct Entity;

struct Editor
{
	Editor(Engine* enginePtr);

	void tick();
	void render();

	Entity* selectedEntity = nullptr;
	float distanceToSelected = 0;
	glm::vec3 grabOffset;
	glm::vec3 oldPos;
	btQuaternion grabRotation;
	float grabWorldYaw;

	Engine* engine;
	bool inEditor = 0;
};