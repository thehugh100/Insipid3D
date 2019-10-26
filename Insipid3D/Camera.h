#pragma once
#include <glm/glm.hpp>

struct Engine;

struct Camera
{
	Camera(Engine* engine);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	bool worldToScreen(glm::vec3 point, glm::vec2& coords);

	void update();

	float fov;
	float zNear, zFar;

	glm::vec3 pos;
	glm::vec3 ang;
	glm::vec3 lookVec;
	glm::vec3 upVec;
	Engine* engine;

	glm::vec2 mousePos;
	glm::vec2 mousePosOld;
	glm::vec2 mouseDelta;
};
