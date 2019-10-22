#include "CollisionState.h"
#include <assimp/mesh.h>
#include "map.h"

CollisionState::CollisionState(Map* map)
	:map(map)
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();

	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setGravity(btVector3(0, -35, 0));
}

void CollisionState::loadMesh()
{
	if (map->getMesh() == nullptr)
	{
		std::cout << "Cannot load collision mesh.\r\n";
	}

	for (int i = 0; i < map->getMesh()->meshEntries.size(); ++i)
	{
		aiMesh* pMesh = map->getMesh()->meshEntries[i]->meshRef;

		btTriangleMesh* m = new btTriangleMesh();
		for (int j = 0; j < pMesh->mNumVertices; j += 3)
		{
			btVector3 a(pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z);
			btVector3 b(pMesh->mVertices[j + 1].x, pMesh->mVertices[j + 1].y, pMesh->mVertices[j + 1].z);
			btVector3 c(pMesh->mVertices[j + 2].x, pMesh->mVertices[j + 2].y, pMesh->mVertices[j + 2].z);
			m->addTriangle(a, b, c);
		}

		btTransform	trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(0, 0, 0));
		btCollisionShape * trimeshShape = new btBvhTriangleMeshShape(m, 1);
		btDefaultMotionState * motionstate = new btDefaultMotionState(trans);
		btRigidBody * body = new btRigidBody(0, motionstate, trimeshShape);
		body->setRestitution(0.2);
		body->setFriction(0.9);
		world->addRigidBody(body);
	}
}