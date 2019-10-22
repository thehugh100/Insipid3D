#pragma once

#include "Mesh.h"
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <map>

struct Engine;

struct MeshManager
{
	MeshManager(Engine *enginePtr);
	Mesh* getMesh(std::string fname, unsigned int flags = 0);

	Engine* engine;
	std::vector<Mesh*> meshes;
};
