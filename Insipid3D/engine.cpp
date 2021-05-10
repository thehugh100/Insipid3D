#include "engine.h"
#include "FlyCam.h"
#include <algorithm>
#include <thread>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "EntityClientCam.h"

Engine::Engine()
{
	networkClient = new NetworkClient(this);
	networkServer = new NetworkServer(this);

	variables = new Variables();
	shaderManager = new ShaderManager();
	entityManger = new EntityManager(this);
	textureManager = new TextureManager();
	map = nullptr;
	screen = glm::vec2(640, 480);
	camera = new Camera(this);
	cameraController = new FlyCam(this);
	meshManager = new MeshManager(this);
	editor = new Editor(this);
	input = new Input(this);
	fontManager = new FontManager();
	console = new Console(this);

	deltaTime = 0.01;
	fps = 1;
	averageFps = 1;
	frameNum = 0;
	engineStart = std::chrono::high_resolution_clock::now();
	frameStart = engineStart;
}

bool Engine::loadMap(std::string fname)
{
	map = new Map(fname, this);
	return true;
}

Map* Engine::getMap()
{
	return map;
}

void Engine::setScreenSize(glm::vec2 screenSize)
{
	screen = screenSize;
}

void Engine::startFrame()
{
	frameStart = std::chrono::high_resolution_clock::now();
}

void Engine::endFrame()
{
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - frameStart);
	deltaTime = time_span.count();
}

void Engine::tick()
{
	if (deltaTime != 0.0f)
		fps = 1.0f / deltaTime;

	averageFps += (fps - averageFps) * 0.01f;

	if (console != nullptr)
		console->tick();

	if (input != nullptr)
		input->tick(frameNum);

	if (camera != nullptr)
		camera->update();

	if (cameraController != nullptr)
		cameraController->control();

	if (entityManger != nullptr)
		entityManger->tick();

	if (editor != nullptr)
		editor->tick();

	networkServer->tick(deltaTime);
	if (networkClient != nullptr)
		networkClient->tick(deltaTime);

	++frameNum;
}

void Engine::render()
{
	if (entityManger != nullptr)
		entityManger->render();

	if (editor != nullptr)
		editor->render();

	if (console != nullptr)
		console->render();
}

void Engine::drawDebugText(Font* f, float x, float y, std::string text, glm::vec3 col)
{
	f->renderText(shaderManager->getShader("shaders/font"), text, x, y, 1, col, glm::ortho(0.f, screen.x, 0.f, screen.y));
}

float Engine::getElapsedTimeMS()
{
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - engineStart);
	return time_span.count() * 1000.0f;
}
