#pragma once
#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include "render.h"
#include "globals.h"
#include "HUtils.h"
#include "input.h"

using namespace std;

struct CEntity
{
	HUtils::XYZ pos;
	virtual void tick()
	{

	}
	virtual void render()
	{

	}
	virtual std::string getClass()
	{
		return "CEntity";
	}
};
