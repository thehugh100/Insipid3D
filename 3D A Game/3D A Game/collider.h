#pragma once
#include "model.h"
#include "entityInterop.h"
#include "entity.h"
#include "HUtils.h"
#include "entityBaseMesh.h"
#include "log.h"
struct CCollider
{
	entBaseMesh *entToMesh(CEntity *in)
	{
		if (in == nullptr)
		{
			alert("in is nullptr in CCollider::*entToMesh(CEntity *in)");
			return nullptr;
		}
		return static_cast<entBaseMesh*>(in);
	}
	trace findCollision(HUtils::XYZ pos, HUtils::XYZ dir, CModel *model)
	{
		trace t;
		if (model == nullptr)
		{
			alert("CModel *model is nullptr in CCollider::findCollision(HUtils::XYZ pos, HUtils::XYZ dir, CModel *model)");
			t.traceFailed = 1;
			t.didHit = 0;
			return t;
		}
		t = rayTrace(pos, dir, model);
		return t;
	}
};
CCollider *collider;