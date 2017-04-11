#include "globals.h"

void CEngineGlobals::init()
{
	windowName = "A Game";
	windowWidth = 1600;
	windowHeight = 900;
	yaw = 0;
	pitch = 0;
	tick = 0;
	timeDeltaAverageRate = 30;
	cX = 0;
	cY = 0;
	cZ = 0;
	cVX = 0;
	cVY = 0;
	cVZ = 0;
	gravity = 0.00012;
	tickTime = 8000;
}