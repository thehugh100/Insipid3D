#pragma once
#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include "render.h"
#include "globals.h"
#include "entity.h"
#include "entityManager.h"
#include <thread>
#include "input.h"
#include "entityPlayer.h"
#include "entityWorld.h"
#include "entityBouncyBall.h"
#include "model.h"
#include "entityInterop.h"
#include <chrono>
#include "entityEnviroment.h"
#include "collider.h"
struct CEngine
{
	CEngine(int argc, char **argv)
	{
		srand(time(0));
		globals = new CEngineGlobals();
		globals->init();
		camera = new CCameraController;
		renderer = new CRenderer(argc, argv);
		entityManager = new CEntityManager();
		entityInterop = new CEntityInterop();
		collider = new CCollider();
		input = new CInput();
		
		engineRunning = 1;
	}
	/*Called before the engine enters its main loop but after the renderer and entity manager have been initiated*/
	void engineInit()
	{
		renderer->detatch();
		input->init();
		dynamic_cast<entWorld*>(entityManager->addEntity(new entWorld(new CModel("models\\crossfire\\crossfire.obj", HUtils::XYZ(0, 0, 0), "models\\crossfire\\", 1))))->mesh->mapColorize();
		entityManager->addEntity(new entPlayer(HUtils::XYZ(-1,0.2,-1)));
		entityManager->addEntity(new entEnviroment(0.7, 0.7, 0.95, 0.01f));

		camera->detatch();
	}
	void run()
	{
		engineInit();
		while (engineRunning)
		{
			std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
			input->run();
			entityManager->tick();
			std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> dif = end - begin;
			globals->timeDelta = dif.count();
			globals->timeDeltaAccum += globals->timeDelta;
			globals->tick++;
			if (globals->tick % globals->timeDeltaAverageRate == 0)
			{
				globals->averageTimeDelta = globals->timeDeltaAccum / (float)globals->timeDeltaAverageRate;
				globals->timeDeltaAccum = 0;
				cout << globals->timeDelta << "ms Average: " << globals->averageTimeDelta << "\r";
			}
			//cout << globals->timeDelta << endl;
		}
	}
	bool engineRunning;
};
CEngine *engine;