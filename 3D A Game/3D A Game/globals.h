#pragma once
#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include <chrono>
struct CEngineGlobals
{
	std::string windowName;
	float windowWidth;
	float windowHeight;
	
	//this is painful
	float cX, cY, cZ;

	float yaw;
	float pitch;
	long int tick;
	double timeDelta;
	double averageTimeDelta;
	double timeDeltaAccum;
	float gravity;
	/*Game ticks untill average*/
	int timeDeltaAverageRate;
	CEngineGlobals()
	{
		
	}
	void init();
};
static CEngineGlobals *globals;