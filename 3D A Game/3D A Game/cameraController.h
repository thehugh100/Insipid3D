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
		rumble = 0;
		sway = HUtils::XYZ(0, 1, 0);
	}
	void update(HUtils::XYZ pos_, HUtils::XYZ vel)
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
		lookPos.x = pos_.x + cos(HUtils::clamp(yaw + yawAdditive, -360, 360) * 3.1415 / 180.0f);
		lookPos.z = pos_.z + sin(HUtils::clamp(yaw + yawAdditive, -360, 360) * 3.1415 / 180.0f);
		lookPos.y = pos_.y + tan(HUtils::clamp(pitch + pitchAdditive, -89.9, 89.9) * 3.1415 / 180.0f);

		if (pitch > -80 && pitch < 80)
			upVec = HUtils::XYZ(0, 1, 0) + (vel * HUtils::XYZ(1, 0, 1)) * 2.2;
		else
			upVec = HUtils::XYZ(0, 1, 0);

		pitchAdditive = HUtils::clamp(pitchAdditive, -90, 90);
		pitchAdditive /= 1.1;
		yawAdditive /= 1.1;
		rumble /= 1.045;
		normalizedLookDir = HUtils::XYZ(cos((yaw) * 3.1415 / 180.0f), tan(pitch * 3.1415 / 180.0f), sin((yaw) * 3.1415 / 180.0f)).normalized();
	}

	void inputThread()
	{
		while (1)
		{
			update(HUtils::XYZ(globals->cX, globals->cY, globals->cZ),  HUtils::XYZ(globals->cVX, globals->cVY, globals->cVZ));

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
	HUtils::XYZ vel;
	HUtils::XYZ sway;

	float pitch = 0, yaw = 0;
	float mouseDx, mouseDy;
	float mouseX, mouseY;
	float oldMouseX, oldMouseY;
	bool lockMouse = 1;
	float rumble = 0;
	float sensitivity = 0.26;
	float pitchAdditive = 0;
	float yawAdditive = 0;
};
CCameraController *camera;