#include "EntityClientCam.h"
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

EntityClientCam::EntityClientCam(glm::vec3 pos)
	:pos(pos)
{
	entityType = "EntityClientCam";
	vars.registerVal("point", Serializer(&pos));

	active = 1;
}

void EntityClientCam::tick()
{
	if (active)
	{
	}
}

void EntityClientCam::render()
{
	if (active)
	{
		GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
		glUseProgram(flatShader);
		glActiveTexture(GL_TEXTURE0);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(0.5));

		glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(0.8,0.8,1)));
		editorMesh->render();

		//glDepthMask(GL_FALSE);
		//glDepthMask(GL_TRUE);
	}
}

void EntityClientCam::init()
{
	editorMesh = engine->meshManager->getMesh("models/editor/sphere.glb");
	initialised = 1;
}

void EntityClientCam::remove()
{
}
