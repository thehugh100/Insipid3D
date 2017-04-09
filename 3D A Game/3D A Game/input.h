#pragma once
#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include "render.h"
#include "globals.h"
#include "entity.h"
#include "HUtils.h"
#include <mutex>

struct CInput
{
	CInput()
	{
	}
	void init()
	{
		keyboard = HUtils::keyboard;
		mouse = HUtils::mouse;
		mouse->setRelativeWindow(globals->windowName);
	}
	void run()
	{
		keyboard->run();
		mouse->run();
	}
	HUtils::CKeyboardController *keyboard;
	HUtils::CMouseController *mouse;
	std::mutex inputGuard;
};
CInput *input;