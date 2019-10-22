#pragma once
#include <iostream>
#include <string>
#include <string.h>

#include "EntityTraits.h"

struct Engine;

struct Entity
{
	Entity();
	Entity(Engine* engine);
	virtual void tick() = 0;
	virtual void render() = 0;
	virtual void init() = 0;
	
	EntityTraits entityTraits;

	Engine* engine;
};