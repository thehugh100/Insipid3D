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
#include "entityMine.h"
#include <minmax.h>

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
		entityManager->addEntity(new entMine(HUtils::XYZ(2, 4, 16)));
		entityManager->addEntity(new entMine(HUtils::XYZ(-20, 5, 6)));
		entityManager->addEntity(new entMine(HUtils::XYZ(8.5, 5, 34)));

		camera->detatch();
	}
	void run()
	{
		engineInit();
		while (engineRunning)
		{
			std::chrono::high_resolution_clock::time_point start, finish;
			start = std::chrono::high_resolution_clock::now();

			//cout << "lol";
			//cout << "lol";

			input->run();
			entityManager->tick();

			finish = std::chrono::high_resolution_clock::now();

			// larger it is, the bigger the time delta needs to be
			double dif = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

			// processWait is the base speed for one tick
			// if the machine is under performing, dif will be higher than processWait
			// this yields a fraction > 1, so will 'speed' physics up
			// similar is true for over performing machines, physics will be slowed down
			globals->timeDelta = dif / globals->tickTime;

			globals->timeDeltaAccum += globals->timeDelta;
			globals->tick++;
			if (globals->tick % globals->timeDeltaAverageRate == 0)
			{
				globals->averageTimeDelta = globals->timeDeltaAccum / (float)globals->timeDeltaAverageRate;
				globals->timeDeltaAccum = 0;
				//cout << globals->timeDelta << "ms Average: " << globals->averageTimeDelta << "\r";
				//cout << us << " ns" << "\r";
			}
			/*60 ticks a second*/
			/*while (std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - begin).count() < 4)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}*/

			//cout << globals->timeDelta << endl;
		}
	}
	bool engineRunning;
};
CEngine *engine;