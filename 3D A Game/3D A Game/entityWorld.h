#pragma once
#include "entity.h"
#include "engine.h"
#include "HUtils.h"
#include "globals.h"
#include "entityManager.h"
#include "input.h"
#include "model.h"
#include "entityBaseMesh.h"

struct entWorld : entBaseMesh
{
	entWorld()
	{

	}
	entWorld(CModel *mesh_)
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
	virtual std::string getClass()
	{
		return "entWorld";
	}
	std::string meshFileName;
	CModel *mesh;
};