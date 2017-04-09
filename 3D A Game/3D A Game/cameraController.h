#pragma once

#include "HUtils.h"
#include "input.h"
#include "globals.h"
#include <minmax.h>
#include <mutex>

struct CCameraController
{
	CCameraController()
	{
		pos = HUtils::XYZ(0, 0, 0);
		lookPos = HUtils::XYZ(0, 0, 0);
		upVec = HUtils::XYZ(0, 1, 0);
		mouseDx = 0;
		mouseDy = 0;
	}
	void update(HUtils::XYZ pos_)
	{
		sensitivity = 0.2;
		//unfreeze mouse
		if (input->keyboard->keyDownEvent('Q'))
			lockMouse = !lockMouse; 
		oldMouseX = mouseX;
		oldMouseY = mouseY;
		mouseX = input->mouse->x;
		mouseY = input->mouse->y;

		float xDif = (mouseX - globals->windowWidth / 2) * sensitivity;
		float yDif = -(mouseY - globals->windowHeight / 2) * sensitivity;

		yaw += xDif;
		pitch += yDif; 

		yaw = fmod(yaw, 360);
		if (pitch > 89.9)
			pitch = 89.9;
		if (pitch < -89.9)
			pitch = -89.9;

		globals->pitch = pitch;
		globals->yaw = yaw;

		if (lockMouse)
		{
			glutWarpPointer(globals->windowWidth / 2, globals->windowHeight / 2);
			SetCursor(NULL);
		}
		else
		{
			ShowCursor(1);
		}

		pos = pos_;
		lookPos.x = pos_.x + cos((yaw) * 3.1415 / 180.0f);
		lookPos.z = pos_.z + sin((yaw) * 3.1415 / 180.0f);
		lookPos.y = pos_.y + tan(pitch * 3.1415 / 180.0f);

		normalizedLookDir = HUtils::XYZ(cos((yaw) * 3.1415 / 180.0f), tan(pitch * 3.1415 / 180.0f), sin((yaw) * 3.1415 / 180.0f)).normalized();
	}

	void inputThread()
	{
		while (1)
		{
			update(HUtils::XYZ(globals->cX, globals->cY, globals->cZ));

			//very bad way to keep time but it will do for now... basically this should be changed to a timer that aims to keep the input thread running at 60hz
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void detatch()
	{
		thread st(&CCameraController::inputThread, this);
		st.detach();
	}

	HUtils::XYZ pos;
	HUtils::XYZ lookPos;
	HUtils::XYZ upVec;
	HUtils::XYZ normalizedLookDir;
	float pitch = 0, yaw = 0;
	float mouseDx, mouseDy;
	float mouseX, mouseY;
	float oldMouseX, oldMouseY;
	bool lockMouse = 1;
	float sensitivity = 0.26;
};
CCameraController *camera;