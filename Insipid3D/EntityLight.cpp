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
	entityType = "EntityLight";
	point = &light->pos;
	vars.registerVal("point", Serializer((glm::vec3*)point));

	vars.registerVal("light_col", Serializer(&light->col));
	vars.registerVal("light_dir", Serializer(&light->dir));
	vars.registerVal("light_intensity", Serializer(&light->intensity));
	vars.registerVal("light_type", Serializer(&light->type));

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

		if (engine->editor->pointSelection == this)
		{
			renderHandles();
		}

		//glDepthMask(GL_FALSE);
		//glDepthMask(GL_TRUE);
	}
}

void EntityLight::init()
{
	editorMesh = engine->meshManager->getMesh("models/editor/sphere.glb");
	initialised = 1;
}

void EntityLight::remove()
{
}
