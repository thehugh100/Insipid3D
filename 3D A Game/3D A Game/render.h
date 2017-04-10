#pragma once
#include <iostream>
#include <windows.h>
#include "GL\freeglut.h"
#include <thread>
#include "HUtils.h"
#include "globals.h"
#include <chrono>
#include "cameraController.h"
#include "entityManager.h"

/*Sorry, not clever enough*/
HUtils::XYZ cameraPos;
HUtils::XYZ lookPos;
HUtils::XYZ cameraVel;
HUtils::XYZ upVec;

struct CRenderer
{
	
	int statusOk = 0;

	CRenderer(int argc, char **argv)
	{
		init(argc, argv);
	}
	static void reshape(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(camera->fov + camera->fovAdditive, (float)w / (float)h, 0.1, 1000000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	static void render()
	{
		ShowCursor(!camera->lockMouse);
		cameraPos = camera->pos;
		lookPos = camera->lookPos;
		cameraVel = camera->vel;
		upVec = camera->upVec;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float shakeFrequency = 180.0f;
		float shakeX = sin((globals->tick - 20) / shakeFrequency) * camera->rumble;
		float shakeY = sin((globals->tick + 1) / shakeFrequency) * camera->rumble;
		float shakeZ = cos((globals->tick + 10) / shakeFrequency) * camera->rumble;

		glLoadIdentity();
		gluLookAt(cameraPos.x + shakeX, cameraPos.y + shakeY, cameraPos.z + shakeZ, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		
		entityManager->render();

		reshape(globals->windowWidth, globals->windowHeight);
		glutSwapBuffers();
		glutPostRedisplay();
	}
	void init(int argc, char **argv)
	{
		glutInit(&argc, argv);
	}
	void run()
	{
		glutInitWindowSize(globals->windowWidth, globals->windowHeight);
		glutInitWindowPosition(10, 10);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
		glutCreateWindow(globals->windowName.c_str());
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_SMOOTH);
		glShadeModel(GL_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		statusOk = 1;
		glutDisplayFunc(&render);
		glutReshapeFunc(&reshape);

		GLuint filter;                      // Which Filter To Use
		GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
		GLuint fogfilter = 3;                    // Which Fog To Use
		GLfloat fogColor[4] = { 0.5f, 0.5f, 0.8, 1.0f };      // Fog Color

		glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
		glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
		glFogf(GL_FOG_DENSITY, 0.003f);              // How Dense Will The Fog Be
		glHint(GL_FOG_HINT, GL_NICEST);          // Fog Hint Value
		glFogf(GL_FOG_START, 600.0f);             // Fog Start Depth
		glFogf(GL_FOG_END, 900.0f);               // Fog End Depth
		glEnable(GL_FOG);
		//glEnable(GLUT_MULTISAMPLE);
		
		glutMainLoop();
	}
	/*Detatch the renderer into a seperate thread*/
	void detatch()
	{
		std::thread q(&CRenderer::run, this);
		q.detach();
		while (!statusOk)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
};
CRenderer *renderer;