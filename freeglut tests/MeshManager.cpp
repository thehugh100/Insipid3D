#include "MeshManager.h"
#include "engine.h"

MeshManager::MeshManager(Engine* enginePtr)
{
	engine = enginePtr;
}

Mesh* MeshManager::getMesh(std::string fname, unsigned int flags)
{
	for (auto& i : meshes)
	{
		if (i->filename == fname)
		{
			std::cout << "Loading Mesh: " << fname << ", already loaded." << std::endl;
			return i;
		}
	}
	Mesh* newMesh = new Mesh(fname.c_str(), engine->textureManager, flags);
	meshes.push_back(newMesh);
	return newMesh;
}
