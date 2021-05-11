#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "GLShader.h"
#include "Mesh.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <string>
#include <string.h>
#include <thread>
#include <math.h>
#include <time.h>
#include <map>
#include <minmax.h>
#include <sstream>
#include <algorithm>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_vector.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "TextureManager.h"
#include "Light.h"
#include "Lightmap.h"
#include "map.h"
#include "engine.h"
#include "Camera.h"
#include "Raytrace.h"
#include "Util.h"

#include "EntityDebugCrate.h"
#include "EntityPhysicsProp.h"
#include "EntityExplosiveBarrel.h"
#include "EntityGrenade.h"
#include "EntityExplosion.h"
#include "Player.h"

#include "Console.h"

#include <assimp\mesh.h>
#include <btBulletDynamicsCommon.h>

Engine* engine;

void render()
{
	while (!glfwWindowShouldClose(engine->window))
	{
		const std::lock_guard<std::mutex> lock(engine->engineLock);
		engine->startFrame();
		glClear(GL_COLOR_BUFFER_BIT);

		if (engine->getMap() != nullptr)
		{
			if (engine->input->mousePressed(GLFW_MOUSE_BUTTON_RIGHT))
			{
				auto world = engine->getMap()->collisionState->world;
				btVector3 start = Util::vec3Conv(engine->camera->pos);
				btVector3 end = Util::vec3Conv(engine->camera->pos + engine->camera->lookVec * 1000.0f);

				btDynamicsWorld::ClosestRayResultCallback r(start, end);
				world->rayTest(start, end, r);

				if (r.hasHit())
				{
					engine->entityManger->addEntity(new EntityExplosion(Util::vec3Conv(r.m_hitPointWorld) - engine->camera->lookVec, 1000.f));
				}
			}
			if (engine->input->keyPressed(GLFW_KEY_Q) && !engine->console->consoleShowing)
			{
				EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(new EntityPhysicsProp("models/crate.glb", engine->camera->pos + engine->camera->lookVec, 80));
				//e->body->setLinearVelocity(Util::vec3Conv(engine->camera->lookVec * 20.0f));
			}
			if (engine->input->keyPressed(GLFW_KEY_B) && !engine->console->consoleShowing)
			{
				engine->entityManger->addEntity(new EntityExplosiveBarrel(engine->camera->pos + engine->camera->lookVec * 2.f));
			}
			if (engine->input->keyPressed(GLFW_KEY_C) && !engine->console->consoleShowing)
			{
				EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(new EntityPhysicsProp("models/box.glb", engine->camera->pos + engine->camera->lookVec, 35));
				e->body->setCcdMotionThreshold(0.2f);
				e->body->setCcdSweptSphereRadius(0.4f);
				//e->body->setLinearVelocity(Util::vec3Conv(engine->camera->lookVec * 20.0f));
			}
			if (engine->input->keyPressed(GLFW_KEY_G) && !engine->console->consoleShowing)
			{
				EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(new
					EntityGrenade(engine->camera->pos));
				e->body->setLinearVelocity(Util::vec3Conv(engine->camera->lookVec * 60.0f));
			}
		}
		engine->tick();

		if(engine->getMap() != nullptr)
			engine->getMap()->collisionState->world->stepSimulation(engine->deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1, 1, 1);

		if (engine->getMap() != nullptr)
			engine->getMap()->render();
		engine->render();

		glfwSwapBuffers(engine->window);
		glfwPollEvents();
		engine->endFrame();
	}
}

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		exit(0);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	engine->input->handleCharacter(codepoint);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	engine->input->handleScroll(yoffset, engine->frameNum);
}

int main(int argc, char** argv)
{
	Console* console = new Console(nullptr);

	*console << "glfwInit" << std::endl;
	glfwSetErrorCallback(&glfwError);
	if (!glfwInit())
		return -1;
	*console << "glfwInit Done" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_REFRESH_RATE, INT_MAX);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "GL", NULL, NULL);
	//GLFWwindow* window = glfwCreateWindow(1920, 1080, "GL", glfwGetPrimaryMonitor(), NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	*console << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	*console << "OpenGL version supported by this platform (" << glGetString(GL_VERSION) << ")" << std::endl;

	glEnable(GL_TEXTURE);
	glEnable(GL_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	engine = new Engine();
	console->engine = engine;
	engine->console = console;
	engine->window = window;
	engine->screen = glm::vec2(width, height);
	engine->input->window = window;
	engine->loadMap("worlds/test_world.glb");

	if (argc > 1)
	{
		std::ostringstream consoleCmd;
		for (int i = 1; i < argc; ++i)
		{
			consoleCmd << argv[i] << (i == (argc - 1) ? "" : " ");
		}
		console->parseCommand(consoleCmd.str());
	}

	//engine->map->addLight(new Light(glm::vec3(-0.5, 8.0, 0.1), glm::vec3(1, 0.3, 0.3), 800.0f));
	//engine->map->addLight(new Light(glm::vec3(1, 6.5, 23), glm::vec3(0.97, 1, 0.9), 300.0f));
	//engine->map->addLight(new Light(glm::vec3(-15, 6.5, 11.3), glm::vec3(0.95, 1, 0.86), 150.0f));

	engine->map->generateLightmap();

	engine->camera->pos = glm::vec3(11, 24.125, 18.);
	engine->camera->ang = glm::vec3(-24, 0, -104);

	//engine->cameraController = new Player(engine);

	//engine->networkClient->connect("");

	render();

	glfwTerminate();
}