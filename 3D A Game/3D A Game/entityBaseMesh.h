/*
	Base class for a mesh entity
*/
#pragma once
#include "entity.h"
#include "engine.h"
#include "HUtils.h"
#include "globals.h"
#include "entityManager.h"
#include "input.h"
#include "model.h"

struct entBaseMesh : CEntity
{
	entBaseMesh()
	{

	}
	entBaseMesh(CModel *mesh_)
	{
		mesh = mesh_;
	}

	virtual void tick()
	{
		/*Static meshes do nothing...*/
	}
	virtual void render()
	{
		//mesh->renderWireFrame();
		mesh->render(1);
	}

	std::string meshFileName;
	CModel *mesh;
};