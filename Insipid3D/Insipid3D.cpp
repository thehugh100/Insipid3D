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
#include "Player.h"

#include <assimp\mesh.h>
#include <btBulletDynamicsCommon.h>

Engine* engine;

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

struct vertexArrayObject
{
	GLuint vao;
	GLuint vbo;

	vertexArrayObject()
	{
	}

	vertexArrayObject(float *v, int size)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, v, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

vertexArrayObject skybox;
unsigned int cubemapTexture;

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glGenerateTextureMipmap(textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);

	return textureID;
}


void render()
{
	while (!glfwWindowShouldClose(engine->window))
	{
		engine->startFrame();
		glClear(GL_COLOR_BUFFER_BIT);

		if (engine->input->keyPressed(GLFW_KEY_Q) && !engine->console->consoleShowing)
		{
			EntityPhysicsProp *e = (EntityPhysicsProp*)engine->entityManger->addEntity(new EntityPhysicsProp("models/crate.glb", engine->camera->pos + engine->camera->lookVec));
			e->body->setLinearVelocity(Util::vec3Conv(engine->camera->lookVec * 20.0f));
		}

		if (engine->input->keyPressed(GLFW_KEY_B) && !engine->console->consoleShowing)
		{
			//EntityPhysicsProp* e = (EntityPhysicsProp*)engine->entityManger->addEntity(new EntityPhysicsProp("models/crate.glb", engine->camera->pos + engine->camera->lookVec));
			//e->body->setLinearVelocity(Util::vec3Conv(engine->camera->lookVec * 20.0f));

			engine->entityManger->addEntity(new EntityExplosiveBarrel(engine->camera->pos + engine->camera->lookVec * 2.f));
		}

		engine->tick();

		engine->getMap()->collisionState->world->stepSimulation(engine->deltaTime);

		glm::vec3 sunPos = glm::vec3(0, 10, 0);
		glm::vec3 cameraPos = engine->camera->pos;

		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = engine->camera->getViewMatrix();
		glm::mat4 proj = engine->camera->getProjectionMatrix();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1, 1, 1);
	
		glDepthMask(GL_FALSE);
		GLuint skyboxShader = engine->shaderManager->getShader("shaders/skybox");
		glUseProgram(skyboxShader);
		glActiveTexture(GL_TEXTURE0);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(view))));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glBindVertexArray(skybox.vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		/*
		glUseProgram(cubemapShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glUniformMatrix4fv(glGetUniformLocation(cubemapShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(cubemapShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubemapShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3fv(glGetUniformLocation(cubemapShader, "cameraPos"), 1, glm::value_ptr(cameraPos));

		broadphase->render();
		//cube->render();
		//glutSolidSphere(0.5, 40, 40);
		*/

		GLuint mapShader = engine->shaderManager->getShader("shaders/map");
		glUseProgram(mapShader);
		glActiveTexture(GL_TEXTURE0);
		glUniformMatrix4fv(glGetUniformLocation(mapShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(mapShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(mapShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform1iARB(glGetUniformLocation(mapShader, "tex"), 0);
		glUniform1iARB(glGetUniformLocation(mapShader, "lightmap"), 1);

		//glEnable(GL_CULL_FACE);

		engine->getMap()->render();

		engine->render();

		rayHit r = RayTrace::rayTrace(engine->camera->pos, engine->camera->lookVec, engine->getMap()->getMesh());

		GLuint flatShader = engine->shaderManager->getShader("shaders/flat");
		glUseProgram(flatShader);
		glActiveTexture(GL_TEXTURE0);
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(flatShader, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3f(glGetUniformLocation(flatShader, "col"), 1., 1., 1. );
		glDepthMask(GL_FALSE);

		glPointSize(4);
		glLineWidth(4);

		glBegin(GL_LINES);
		glVertex3fv(glm::value_ptr(r.pos));
		glVertex3fv(glm::value_ptr(r.pos + r.normal));
		glEnd();

		std::ostringstream dbg;
		dbg << "fps: " << floor(engine->averageFps) << " (" << engine->camera->pos.x << ", " << engine->camera->pos.y << ", " << engine->camera->pos.z << ")";
		dbg << " " << engine->camera->ang.x << ", " << engine->camera->ang.z;

		if (!engine->console->consoleShowing)
		{
			engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), 2, 720 - 18, dbg.str(), glm::vec3(0.f));
			engine->drawDebugText(engine->fontManager->getFont("fonts/Roboto_Mono/RobotoMono-Regular.ttf", 18), 3, 720 - 19, dbg.str());
		}

		glDepthMask(GL_TRUE);

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
	//if (key == GLFW_KEY_E && action == GLFW_PRESS)
		//std::cout << 'E' << std::endl;

	//engine->input->handleKeyPress(key, action);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		exit(0);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	engine->input->keyboardBuffer += codepoint;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	engine->input->handleScroll(yoffset, engine->frameNum);
}

int main(int argc, char** argv)
{
	std::cout << "glfwInit" << std::endl;
	glfwSetErrorCallback(&glfwError);
	if (!glfwInit())
		return -1;
	std::cout << "glfwInit Done" << std::endl;

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
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

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
	engine->window = window;
	engine->screen = glm::vec2(width, height);
	engine->input->window = window;
	engine->loadMap("worlds/test_world.glb");

	engine->map->lights.push_back(Light(glm::vec3(-0.5, 8.0, 0.1), glm::vec3(1, 0.3, 0.3), 800.0f));
	engine->map->lights.push_back(Light(glm::vec3(1, 6.5, 23), glm::vec3(0.97, 1, 0.9), 300.0f));
	engine->map->lights.push_back(Light(glm::vec3(-15, 6.5, 11.3), glm::vec3(0.95, 1, 0.86), 150.0f));
	//engine->map->lights.push_back(Light(glm::vec3(-15.0796, 15.084, 20.8173), 
	//	glm::vec3(1,1,1), 600.0f, LIGHT_SPOT, 
	// 	glm::vec3(0.00649173, -0.371367, 0.928464)));
	engine->map->generateLightmap();

	engine->entityManger->addEntity(new EntityPhysicsProp("models/crate.glb", glm::vec3(-4, 20, 4)));
	engine->entityManger->addEntity(new EntityPhysicsProp("models/crate.glb", glm::vec3(-13.8373, 18.1396, 33.7431)));

	engine->entityManger->addEntity(new EntityPhysicsProp("models/wordart.glb", glm::vec3(0, 40, 0)));

	engine->camera->pos = glm::vec3(11, 24.125, 18.);
	engine->camera->ang = glm::vec3(-24, 0, -104);

	//engine->cameraController = new Player(engine);

	std::vector<std::string> faces
	{
		"textures/hw_sahara/sahara_rt.tga",
		"textures/hw_sahara/sahara_lf.tga",
		"textures/hw_sahara/sahara_up.tga",
		"textures/hw_sahara/sahara_dn.tga",
		"textures/hw_sahara/sahara_ft.tga",
		"textures/hw_sahara/sahara_bk.tga"
	};
	cubemapTexture = loadCubemap(faces);
	skybox = vertexArrayObject(skyboxVertices, sizeof(skyboxVertices));

	render();

	glfwTerminate();
}