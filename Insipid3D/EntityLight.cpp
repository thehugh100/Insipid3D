#include "EntityLight.h"
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

EntityLight::EntityLight(Light *light)
	:light(light)
{
	entityTraits.setTrait("EntityLight");
	active = 1;
}

void EntityLight::tick()
{
	if (active)
	{
		//std::cout << Util::printVec3(light->pos) << std::endl;
	}
}

void EntityLight::render()
{
	if (active && engine->editor->inEditor)
	{
		GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
		glUseProgram(flatShader);
		glActiveTexture(GL_TEXTURE0);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, light->pos);
		model = glm::scale(model, glm::vec3(0.25));

		glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(light->col));
		editorMesh->render();

		glm::vec2 screenPos;
		if (engine->camera->worldToScreen(light->pos, screenPos))
		{
			if (glm::distance(engine->screen / 2.f, screenPos) < 64.0f)
			{
				glm::mat4 arrowIdentity = glm::mat4(1.f);
				glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(0, 1, 0)));
				glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
					1, GL_FALSE, glm::value_ptr(glm::translate(arrowIdentity, light->pos)));
				engine->meshManager->getMesh("models/editor/arrow.glb")->render();

				glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(1, 0, 0)));
				glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
					1, GL_FALSE, glm::value_ptr(glm::rotate(glm::translate(arrowIdentity, light->pos), glm::radians(90.f), glm::vec3(1, 0, 0))));
				engine->meshManager->getMesh("models/editor/arrow.glb")->render();

				glUniform3fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec3(0, 0, 1)));
				glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"),
					1, GL_FALSE, glm::value_ptr(glm::rotate(glm::translate(arrowIdentity, light->pos), glm::radians(90.f), glm::vec3(0, 0, 1))));
				engine->meshManager->getMesh("models/editor/arrow.glb")->render();

			}
		}

		//glDepthMask(GL_FALSE);
		//glDepthMask(GL_TRUE);
	}
}

void EntityLight::init()
{
	editorMesh = engine->meshManager->getMesh("models/editor/sphere.glb");
}

void EntityLight::remove()
{
}
