#include "EntityFlare.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "engine.h"
#include "MeshManager.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

EntityFlare::EntityFlare(glm::vec3 origin, glm::vec3 col)
	:pos(origin), col(col)
{
	entityType = "EntityFlare";
	entityTraits.setTrait("EntityFlare");
}

EntityFlare::EntityFlare()
{
	entityType = "EntityFlare";
	entityTraits.setTrait("EntityFlare");
	pos = glm::vec3(0, 0, 0);
	col = glm::vec3(1, 1, 1);
}

void EntityFlare::tick()
{
}

void EntityFlare::render()
{
	if (active)
	{
		GLuint flatShader = engine->shaderManager->getShader("shaders/tex");
		glUseProgram(flatShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, flareTex);

		glm::quat lQuat = glm::quatLookAt(glm::normalize(-engine->camera->lookVec), glm::vec3(0, 1, 0)) * glm::quat(glm::vec3(3.1415 / 2, 0, 0));

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, pos);
		model = model * glm::mat4_cast(lQuat);
		model = glm::scale(model, glm::vec3(3.33));

		glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(engine->camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(engine->camera->getProjectionMatrix()));
		glUniform4fv(glGetUniformLocation(flatShader, "col"), 1, glm::value_ptr(glm::vec4(col, .333)));

		glDisable(GL_CULL_FACE);
		//glDepthMask(GL_FALSE);

		engine->meshManager->getMesh("models/editor/flat.glb")->renderExternTex(flareTex);

		//glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	}
}

void EntityFlare::init()
{
	initialised = 1;
	flareTex = engine->textureManager->loadTexture("textures/flare.png");
}

void EntityFlare::remove()
{
	active = 0;
}
