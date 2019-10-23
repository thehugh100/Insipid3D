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
	virtual void remove() = 0;
	bool active;
	EntityTraits entityTraits;
	Engine* engine;
};