#pragma once
#include "CameraController.h"
#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <glm/glm.hpp>

struct Engine;

struct Player : CameraController
{
	Player(Engine* engine);
	virtual void control();

	btPairCachingGhostObject* body;

	glm::vec3 pos;
	btTransform transform;

	Engine *engine;
};