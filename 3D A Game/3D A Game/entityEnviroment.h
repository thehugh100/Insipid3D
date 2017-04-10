#pragma once
#include "entity.h"
/*entEnviroment is the entity that will render the world background and sets the sky color*/
struct entEnviroment : CEntity
{
	entEnviroment()
	{
		col[0] = 1;
		col[1] = 1;
		col[2] = 1;
		col[3] = 1;
	}
	entEnviroment(float r, float g, float b, float a)
	{
		col[0] = r;
		col[1] = g;
		col[2] = b;
		col[3] = a;
	}
	void tick()
	{

	}
	void render()
	{
		glClearColor(col[0], col[1], col[2], col[3]);
	}
	virtual std::string getClass()
	{
		return "entEnviroment";
	}
	float col[4];
};