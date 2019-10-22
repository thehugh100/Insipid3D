#pragma once
#include "CameraController.h"

struct FlyCam : CameraController
{
	FlyCam(Engine* engine);
	virtual void control();
};