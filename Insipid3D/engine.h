#pragma once

#include "TextureManager.h"
#include "map.h"
#include "Camera.h"
#include "CameraController.h"
#include "EntityManager.h"
#include "MeshManager.h"
#include "Editor.h"
#include "Input.h"
#include "FontManager.h"
#include "Console.h"
#include <iostream>
#include <string.h>
#include <string>
#include <glm/glm.hpp>
#include <chrono>
#include <map>
#include "ShaderManager.h"
#include "Variable.h"

#include "GL/glew.h"
#include <GLFW/glfw3.h>

struct Engine
{
	Engine();

	bool loadMap(std::string fname);
	Map* getMap();
	void setScreenSize(glm::vec2 screenSize);

	void startFrame();
	void endFrame();
	void tick();
	void render();

	void drawDebugText(Font* f, float x, float y, std::string text, glm::vec3 col = glm::vec3(1.0f));

	float getElapsedTimeMS();

	std::chrono::high_resolution_clock::time_point engineStart;
	std::chrono::high_resolution_clock::time_point frameStart;

	uint64_t frameNum;
	float deltaTime;
	float fps;
	float averageFps;
	glm::vec2 screen;

	//std::map<std::string, GLuint> shaders;
	Map* map;
	TextureManager* textureManager;
	Camera* camera;
	CameraController* cameraController;
	EntityManager* entityManger;
	MeshManager* meshManager;
	Editor* editor;
	Input* input;
	FontManager* fontManager;
	Console* console;
	GLFWwindow* window;
	ShaderManager* shaderManager;
	Variables* variables;
};