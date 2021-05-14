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

#include "MeshManager.h"

EntityClientCam::EntityClientCam(glm::vec3 pos)
	:pos(pos)
{
	entityType = "EntityClientCam";
	lookVec = glm::vec3(0, 0, 0);
	hitPos = glm::vec3(0, 0, 0);

	vars.registerVal("point", Serializer(&pos));
	vars.registerVal("lookVec", Serializer(&lookVec));
	vars.registerVal("hitPos", Serializer(&hitPos));

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

		glDepthMask(GL_FALSE);

		glUseProgram(flatShader);
		glActiveTexture(GL_TEXTURE0);
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.)));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		glUniform3f(glGetUniformLocation(flatShader, "col"), 1., 1., 1.);
		glDepthMask(GL_FALSE);

		glPointSize(4);
		glLineWidth(4);

		glPointSize(4);
		glLineWidth(4);

		glm::vec3 p = pos;
		glm::vec3 n = lookVec * 1.5f;

		glBegin(GL_LINES);
		glVertex3fv(glm::value_ptr(p));
		glVertex3fv(glm::value_ptr(p + n));
		glEnd();
		
		glDepthMask(GL_FALSE);
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
