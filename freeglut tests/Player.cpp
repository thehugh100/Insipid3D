#include "Player.h"
#include "engine.h"
#include "CameraController.h"
#include "Util.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <GLFW/glfw3.h>

Player::Player(Engine* enginePtr)
{
	engine = enginePtr;
	
	glm::vec3 origin = glm::vec3(11, 24.125, 18.);
	pos = origin;
	btTransform t;
	t.setIdentity();
	t.setOrigin(Util::vec3Conv(origin));

	body = new btPairCachingGhostObject();
	body->setCollisionShape(new btCapsuleShape(1.5, 3));
	transform.setIdentity();
	transform.setOrigin(Util::vec3Conv(pos));
	body->setWorldTransform(transform);
	engine->getMap()->collisionState->world->addCollisionObject(body);
	engine->getMap()->collisionState->world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

void Player::control()
{
	float flyingSpeed = 10.;

	glm::vec3 wishDir = glm::vec3(0, 0, 0);
	bool keyPressed = 0;
	bool aP = 0, dP = 0;

	if (glfwGetKey(engine->window, GLFW_KEY_LEFT_SHIFT)) { flyingSpeed *= 2.f; }
	if (glfwGetKey(engine->window, GLFW_KEY_W)) { wishDir += engine->camera->lookVec; keyPressed = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_S)) { wishDir -= engine->camera->lookVec; keyPressed = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_A)) { wishDir += glm::rotateY(glm::vec3(engine->camera->lookVec.x, 0., engine->camera->lookVec.z), 3.1415f / 2.f); keyPressed = 1; aP = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_D)) { wishDir += glm::rotateY(glm::vec3(engine->camera->lookVec.x, 0., engine->camera->lookVec.z), -3.1415f / 2.f); keyPressed = 1; dP = 1; }

	pos.y -= 0.002;

	engine->camera->pos = pos;
	auto qq = body->getOverlappingPairCache();
	
	std::cout << qq->GetCount() << std::endl;

	transform.setOrigin(Util::vec3Conv(pos));
	body->setWorldTransform(transform);
}
