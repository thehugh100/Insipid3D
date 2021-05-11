#pragma once
#include <btBulletDynamicsCommon.h>

struct Map;

struct CollisionState
{
	CollisionState(Map* map);
	~CollisionState();
	void loadMesh();

	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btCollisionConfiguration* collisionConfig;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* solver;

	Map* map;
};