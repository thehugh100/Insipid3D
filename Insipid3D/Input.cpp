#include "Input.h"
#include <iostream>

Input::Input()
{
	window = nullptr;
	backspacePressedAt = 0;
	rolloverSpeed = 0.4;
	for (int i = 0; i < GLFW_KEY_LAST; ++i)
		keyState[i] = GLFW_RELEASE;
}

void Input::handleKeyPress(int key, int action)
{
	keyStateOld[key] = keyState[key];
	keyState[key] = action;
}

void Input::handleScroll(float newScollOffsetY, uint64_t frame)
{
	scrollOffsetY = newScollOffsetY;
	scrollEventAge = frame;
}

void Input::tick(uint64_t frameNum)
{
	for (int i = 32; i < GLFW_KEY_LAST; ++i)
	{
		keyStateOld[i] = keyState[i];
		keyState[i] = glfwGetKey(window, i);
	}
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
	{
		mouseStateOld[i] = mouseState[i];
		mouseState[i] = glfwGetMouseButton(window, i);
	}

	if (frameNum > scrollEventAge)
		scrollOffsetY = 0;

	if (keyPressed(GLFW_KEY_BACKSPACE))
	{
		backspacePressedAt = glfwGetTime();
		rolloverSpeed = 0.4;
	}

	if (keyDown(GLFW_KEY_BACKSPACE))
	{
		if (keyPressed(GLFW_KEY_BACKSPACE) || (glfwGetTime() > backspacePressedAt + rolloverSpeed))
		{
			if (keyboardBuffer.size() > 0)
				keyboardBuffer = keyboardBuffer.substr(0, keyboardBuffer.length() - 1);
			backspacePressedAt = glfwGetTime();
			if(!keyPressed(GLFW_KEY_BACKSPACE))
				rolloverSpeed = 0.05;
		}
	}
}

bool Input::keyDown(int key)
{
	return keyState[key] == GLFW_PRESS;
}

bool Input::keyPressed(int key)
{
	return keyState[key] == GLFW_PRESS && keyStateOld[key] == GLFW_RELEASE;
}

bool Input::mouseDown(int key)
{
	return mouseState[key] == GLFW_PRESS;
}

bool Input::mousePressed(int key)
{
	return mouseState[key] == GLFW_PRESS && mouseStateOld[key] == GLFW_RELEASE;
}
