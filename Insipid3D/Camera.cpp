#include "Camera.h"
#include "engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Variable.h"

Camera::Camera(Engine* engine)
	:engine(engine)
{
	pos = glm::vec3(0, 0, 0);
	ang = glm::vec3(0, 0, 0);
	lookVec = glm::vec3(1, 0, 0);
	upVec = glm::vec3(0, 1, 0);
	fov = 80;
	zNear = 0.01f;
	zFar = 10000.f;

	mousePos = glm::vec2(0);
	mousePosOld = mousePos;
	mouseDelta = glm::vec2(0);
}

glm::mat4 Camera::getViewMatrix()
{
	lookVec = glm::normalize(glm::vec3(sin(glm::radians(ang.z)), tan(glm::radians(ang.x)), cos(glm::radians(ang.z))));
	return glm::lookAt(pos, pos + lookVec, upVec);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(fov), 1.0f * engine->screen.x / engine->screen.y, zNear, zFar);
}

bool Camera::worldToScreen(glm::vec3 point, glm::vec2& coords)
{
	glm::vec4 clipSpacePos = getProjectionMatrix() * (getViewMatrix() * glm::vec4(point, 1.0));
	glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;
	coords = ((glm::vec2(ndcSpacePos) + 1.0f) / 2.0f) * engine->screen;

	if (clipSpacePos.w < 0 || coords.x < 0 || coords.x > engine->screen.x || coords.y < 0 || coords.y > engine->screen.y)
		return false;

	return true;
}

void Camera::update()
{
	double mX, mY;
	glfwGetCursorPos(engine->window, &mX, &mY);
	mousePosOld = mousePos;
	mousePos = glm::vec2(mX, mY);
	mouseDelta = mousePosOld - mousePos;

	float sensitivity = engine->variables->getVarOrCreate("sensitivity", "0.18", Variable::valFloat)->getFloat();
	fov = engine->variables->getVarOrCreate("fov", "80", Variable::valFloat)->getFloat();

	ang.x += sensitivity * mouseDelta.y;
	ang.z += sensitivity * mouseDelta.x;

	ang.x = glm::clamp(ang.x, -89.9f, 89.9f);
}
