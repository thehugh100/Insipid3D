#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <string>

struct Input
{
	Input();
	void handleKeyPress(int key, int action);
	void handleScroll(float newScollOffsetY, uint64_t frame);

	void tick(uint64_t frameNum);

	bool keyDown(int key);
	bool keyPressed(int key);

	bool mouseDown(int key);
	bool mousePressed(int key);

	GLFWwindow* window;
	
	float scrollOffsetY;
	uint64_t scrollEventAge;

	std::string keyboardBuffer;
	int keyboardBufferCursorPos;
	float backspacePressedAt;
	float rolloverSpeed;

	int keyState[GLFW_KEY_LAST];
	int keyStateOld[GLFW_KEY_LAST];

	int mouseState[GLFW_MOUSE_BUTTON_LAST];
	int mouseStateOld[GLFW_MOUSE_BUTTON_LAST];
};