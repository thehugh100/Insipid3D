#include "FlyCam.h"
#include "engine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <GLFW/glfw3.h>

FlyCam::FlyCam(Engine* enginePtr)
{
	engine = enginePtr;
}

void FlyCam::control()
{
	float flyingSpeed = engine->variables->getVarOrCreate("flycam_speed", "10.0", Variable::valFloat)->getFloat();

	glm::vec3 wishDir = glm::vec3(0, 0, 0);
	bool keyPressed = 0;
	bool aP = 0, dP = 0;

	if (glfwGetKey(engine->window, GLFW_KEY_LEFT_SHIFT)) { flyingSpeed *= engine->variables->getVarOrCreate("flycam_speed_sprint_multiplier", "2.0", Variable::valFloat)->getFloat(); }
	if (glfwGetKey(engine->window, GLFW_KEY_W)) { wishDir += engine->camera->lookVec; keyPressed = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_S)) { wishDir -= engine->camera->lookVec; keyPressed = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_A)) { wishDir += glm::rotateY(glm::vec3(engine->camera->lookVec.x, 0., engine->camera->lookVec.z), 3.1415f / 2.f); keyPressed = 1; aP = 1; }
	if (glfwGetKey(engine->window, GLFW_KEY_D)) { wishDir += glm::rotateY(glm::vec3(engine->camera->lookVec.x, 0., engine->camera->lookVec.z), -3.1415f / 2.f); keyPressed = 1; dP = 1; }
	
	if (keyPressed && !(aP && dP) && !engine->console->consoleShowing)
	{
		if (wishDir != glm::vec3(0.))
			engine->camera->pos += glm::normalize(wishDir) * flyingSpeed * engine->deltaTime;
	}
}
