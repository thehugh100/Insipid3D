#include "Raytrace.h"

inline int RayTrace::rayIntersectsTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float* intersection)
{
	glm::vec3 e1(v1 - v0);
	glm::vec3 e2(v2 - v0);

	const float epsilon = 0.000001f;

	glm::vec3 P, Q;
	float i;
	float t;

	P = glm::cross(dir, e2);
	float det = glm::dot(e1, P);

	if ((det > -epsilon) && (det < epsilon))
	{
		return 0;
	}
	float invDet = 1.0f / det;

	glm::vec3 T = origin - v0;

	float u = glm::dot(T, P) * invDet;

	if ((u < 0.0f) || (u > 1.0f))
	{
		return 0;
	}

	Q = glm::cross(T, e1);

	float v = glm::dot(dir, Q) * invDet;

	if (v < 0.f || u + v  > 1.f) return 0;

	t = glm::dot(e2, Q) * invDet;

	if (t > epsilon)
	{
		if (intersection)
		{
			*intersection = t;
		}
		return true;
	}

	return false;
}

rayHit RayTrace::rayTrace(glm::vec3 origin, glm::vec3 rayDir, Mesh* mesh)
{
	rayHit r;
	r.hit = 0;
	r.distance = 1000000000;

	if (mesh == nullptr || mesh->scene == nullptr)
	{
		std::cout << "Raytrace failed, mesh was nullptr." << std::endl;
		return r;
	}

	for (int i = 0; i < mesh->scene->mNumMeshes; ++i)
	{
		aiMesh* m = mesh->scene->mMeshes[i];
		glm::vec3 tri[3];

		for (int j = 0; j < m->mNumFaces; ++j)
		{
			for (int fi = 0; fi < 3; fi++)
			{
				tri[fi] = glm::vec3(m->mVertices[m->mFaces[j].mIndices[fi]].x, m->mVertices[m->mFaces[j].mIndices[fi]].y, m->mVertices[m->mFaces[j].mIndices[fi]].z);
			}

			float dist;
			if (rayIntersectsTriangle(origin, rayDir, tri[0], tri[1], tri[2], &dist))
			{
				r.hit = 1;
				if (dist < r.distance)
				{
					r.distance = dist;
					r.pos = origin + (rayDir * dist);
					r.normal = glm::triangleNormal(tri[0], tri[1], tri[2]);
				}
			}
		}
	}
	return r;
}