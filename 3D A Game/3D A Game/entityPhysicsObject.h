#pragma once
#include "entity.h"
#include "engine.h"
#include "HUtils.h"
#include "globals.h"
#include "entityManager.h"
#include "input.h"
#include <chrono>
#include "entityInterop.h"
#include "model.h"
#include "entityWorld.h"

struct entPhysicsObject : CEntity
{
	entPhysicsObject()
	{
		health = 100;
		active = 1;
		onGround = 0;
	}
	virtual void simulatePhysics()
	{

	}
	virtual void tick()
	{
		simulatePhysics();
	}
	virtual void render()
	{

	}
	virtual std::string getClass()
	{
		return "entPhysicsObject";
	}
	virtual void takeDamage(float amount)
	{
		health -= amount;
		health = max(0, health);
		if (health == 0)
		{
			active = 0;
		}
	}
	float health;
	bool active;
	bool onGround;
};