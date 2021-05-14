#include "EntityPoint.h"
#include "Util.h"
#include "engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "MeshManager.h"

EntityPoint::EntityPoint()
{
	entityTraits.setTrait("Point");
}

void EntityPoint::renderHandles()
{
	GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
	glUseProgram(flatShader);
	glActiveTexture(GL_TEXTURE0);
	glm::mat4 arrowIdentity = glm::mat4(1.f);
	glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(0, 1, 0)));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
		1, GL_FALSE, glm::value_ptr(glm::translate(arrowIdentity, *point)));
	engine->meshManager->getMesh("models/editor/arrow.glb")->render();

	glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(1, 0, 0)));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
		1, GL_FALSE, glm::value_ptr(glm::rotate(glm::translate(arrowIdentity, *point), glm::radians(90.f), glm::vec3(0, 0, 1))));
	engine->meshManager->getMesh("models/editor/arrow.glb")->render();

	glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(0, 0, 1)));
	glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
		1, GL_FALSE, glm::value_ptr(glm::rotate(glm::translate(arrowIdentity, *point), glm::radians(90.f), glm::vec3(1, 0, 0))));
	engine->meshManager->getMesh("models/editor/arrow.glb")->render();
}

glm::vec3 EntityPoint::getPos()
{
	return *point;
}

void EntityPoint::setPos(glm::vec3 in)
{
	*point = in;
}
