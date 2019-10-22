#pragma once

struct Engine;

struct CameraController
{
	virtual void control() = 0;
	Engine* engine;
};