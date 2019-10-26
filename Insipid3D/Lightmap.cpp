#include "Lightmap.h"
#include <stb_image_write.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include "Raytrace.h"
#include "Util.h"

VisibleLumel::VisibleLumel(LightmapLumel* lumel, float distance, glm::vec3 dir)
	:lumel(lumel), distance(distance), dir(dir)
{

}

inline bool LightmapGenerator::SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 A, glm::vec3 B)
{
	glm::vec3 cp1 = glm::cross(B - A, p1 - A);
	glm::vec3 cp2 = glm::cross(B - A, p2 - A);

	if (glm::dot(cp1, cp2) >= -0.01) return true;
	return false;
}

inline bool LightmapGenerator::PointInTriangle(glm::vec3 TriangleVectors[3], glm::vec3 P)
{
	glm::vec3 A = TriangleVectors[0], B = TriangleVectors[1], C = TriangleVectors[2];
	if (SameSide(P, A, B, C) && SameSide(P, B, A, C) && SameSide(P, C, A, B))
	{
		glm::vec3 vc1 = glm::cross(A - B, A - C);
		if (glm::abs(glm::dot(A - P, vc1)) <= .1f)
			return true;
	}

	return false;
}

inline bool LightmapGenerator::inside_trigon(glm::vec2 s, glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
	float as_x = s.x - a.x;
	float as_y = s.y - a.y;

	bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

	if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0 == s_ab) return false;

	if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0 != s_ab) return false;

	return true;
}

inline void LightmapGenerator::barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float& u, float& v, float& w)
{
	glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.0f - v - w;
}

inline void LightmapGenerator::setAt(int x, int y, int lightMapRes, int* data, glm::vec3 col)
{
	data[((y * lightMapRes * 3) + x * 3)] += col.x;
	data[((y * lightMapRes * 3) + x * 3) + 1] += col.y;
	data[((y * lightMapRes * 3) + x * 3) + 2] += col.z;
}

glm::vec3 LightmapGenerator::getAt(int x, int y, int lightMapRes, int* data)
{
	glm::vec3 ret;
	ret.x = data[((y * lightMapRes * 3) + x * 3)];
	ret.y = data[((y * lightMapRes * 3) + x * 3) + 1];
	ret.z = data[((y * lightMapRes * 3) + x * 3) + 2];
	return ret;
}

inline btDynamicsWorld::ClosestRayResultCallback fastRaytrace(btVector3 start, btVector3 end, btDynamicsWorld* world)
{
	btDynamicsWorld::ClosestRayResultCallback r(start, end);
	world->rayTest(start, end, r);
	return r;
}

void LightmapGenerator::buildLightmapData(Mesh* mesh, std::vector<LightmapFace>* lightmapFaces)
{
	int meshCount = mesh->scene->mNumMeshes;
	for (int i = 0; i < meshCount; ++i)
	{
		aiMesh* m = mesh->scene->mMeshes[i];
		LightmapFace l;

		for (int j = 0; j < m->mNumFaces; ++j)
		{
			for (int fi = 0; fi < 3; fi++)
			{
				l.tri[fi] = glm::vec3(m->mVertices[m->mFaces[j].mIndices[fi]].x, m->mVertices[m->mFaces[j].mIndices[fi]].y, m->mVertices[m->mFaces[j].mIndices[fi]].z);
				l.uvs[fi] = glm::vec2(m->mTextureCoords[0][m->mFaces[j].mIndices[fi]].x, m->mTextureCoords[0][m->mFaces[j].mIndices[fi]].y);

				l.minX = glm::min(l.uvs[fi].x, l.minX);
				l.maxX = glm::max(l.uvs[fi].x, l.maxX);
				l.minY = glm::min(l.uvs[fi].y, l.minY);
				l.maxY = glm::max(l.uvs[fi].y, l.maxY);
			}
			lightmapFaces->push_back(l);
		}
	}
}

void LightmapGenerator::lightmapCalc(Map* map, int lightMapRes, std::vector<Light> lights, TextureManager* textureManager)
{
	Mesh* mesh = map->getMesh();
	int meshCount = mesh->scene->mNumMeshes;
	float lightMapStep = 1.0f / lightMapRes;

	int* lightmap = new int[lightMapRes * lightMapRes * 3];
	for (int i = 0; i < lightMapRes * lightMapRes * 3; ++i) lightmap[i] = 0;

	LightmapLumel nullface = LightmapLumel();
	nullface.null = 1;

	std::vector<LightmapFace> lightmapFaces;
	buildLightmapData(mesh, &lightmapFaces);
	LightmapLumel * lightmapLumelXY = new LightmapLumel[lightMapRes * lightMapRes];

	for (float v = 0; v < 1.0f; v += lightMapStep)
	{
		std::cout << "Lightmap Preprocess, " << (v * 100.0f) << "%     \r";
		for (float u = 0; u < 1.0f; u += lightMapStep)
		{
			int x = u * lightMapRes;
			int y = v * lightMapRes;

			lightmapLumelXY[y * lightMapRes + x] = nullface;
			LightmapLumel* cl = &lightmapLumelXY[y * lightMapRes + x];
			glm::vec2 uv = glm::vec2(u, v) + glm::vec2(lightMapStep * .5f);
			for (auto& i : lightmapFaces)
			{
				if (u > i.maxX || u < i.minX || v > i.maxY || v < i.minY)
					continue;

				if (inside_trigon(uv, i.uvs[0], i.uvs[1], i.uvs[2]))
				{
					barycentric(uv, i.uvs[0], i.uvs[1], i.uvs[2], cl->bu, cl->bv, cl->bw);

					//std::cout << cl->bu << " " << cl->bv << " " << cl->bw << std::endl;

					cl->uv = uv;
					cl->wPos = cl->bu * i.tri[0] + cl->bv * i.tri[1] + cl->bw * i.tri[2];
					i.normal = glm::triangleNormal(i.tri[0], i.tri[1], i.tri[2]) * 0.05f;
					cl->face = &i;
					cl->null = 0;
				}
			}
		}
	}
	std::cout << "Lightmap Preprocess, 100%" << "    \r\n";

	for (float v = lightMapStep; v < 1.0f - lightMapStep; v += lightMapStep)
	{
		std::cout << "Finding missing lumels, " << (v * 100.0f) << "%    \r";
		for (float u = lightMapStep; u < 1.0f - lightMapStep; u += lightMapStep)
		{
			int x = u * lightMapRes;
			int y = v * lightMapRes;

			LightmapLumel* cl = &lightmapLumelXY[y * lightMapRes + x];
			glm::vec2 uv = glm::vec2(u, v) + glm::vec2(lightMapStep * .5f);
			if (cl->null)
			{
				//if(lightmapLumelXY[y * lightMapRes + x].null)
				bool found = 0;
				for (int kX = -1; kX < 2; ++kX)
				{
					for (int kY = -1; kY < 2; ++kY)
					{
						LightmapLumel* i = &lightmapLumelXY[(y + kY) * lightMapRes + (x + kX)];
						if (!i->null && !i->expandedLumel)
						{
							found = 1;
							cl->uv = uv;
							barycentric(uv, i->face->uvs[0], i->face->uvs[1], i->face->uvs[2], cl->bu, cl->bv, cl->bw);

							cl->wPos = cl->bu * i->face->tri[0] + cl->bv * i->face->tri[1] + cl->bw * i->face->tri[2];
							cl->face = i->face;
							cl->null = 0;
							cl->expandedLumel = 1;
							break;
						}
					}
					if (found) break;
				}
			}
		}
	}
	std::cout << "Finding missing lumels, 100%    \r\n";

	/*for (float v = 0; v < 1.0f; v += lightMapStep)
	{
		std::cout << "Calculating visible lumels, " << (v * 100.0f) << "%\r";
		for (float u = 0; u < 1.0f; u += lightMapStep)
		{
			int x = u * lightMapRes;
			int y = v * lightMapRes;

			LightmapLumel* cl = &lightmapLumelXY[y * lightMapRes + x];
			LightmapFace* i = cl->face;


			for (int j = 0; j < lightMapRes * lightMapRes; j++)
			{
				if (lightmapLumelXY[j].null) continue;

				LightmapLumel *jL = &lightmapLumelXY[j];
				float distanceToLumel = glm::distance(jL->wPos, cl->wPos);

				if (distanceToLumel > 100)
					continue;

				glm::vec3 dirToLumel = glm::normalize(jL->wPos - cl->wPos);

				//if (glm::dot(dirToLumel*-1.f, jL->face->normal) < 0.005)
					//continue;

				float bestDist = 100000;
				float dist = 0;
				for (auto& fcs : lightmapFaces)
				{
					if (!rayIntersectsTriangle(cl->wPos + dirToLumel * .15f, dirToLumel, fcs.tri[0], fcs.tri[1], fcs.tri[2], &dist))
					{
						continue;
					}
					if (dist < distanceToLumel)
						break;
					if (dist < bestDist)
						bestDist = dist;
				}

				if(abs(bestDist - distanceToLumel) < .5f)
					cl->visibleLumels.push_back(VisibleLumel(jL, distanceToLumel, dirToLumel));
			}
		}
	}*/


	glm::vec3 sunDir = -map->sunVec;

	for (float v = 0; v < 1.0f; v += lightMapStep)
	{
		std::cout << "Processing Lightmap, " << (v * 100.0f) << "%    \r";

		for (float u = 0; u < 1.0f; u += lightMapStep)
		{
			int x = u * lightMapRes;
			int y = v * lightMapRes;

			LightmapLumel* cl = &lightmapLumelXY[y * lightMapRes + x];
			LightmapFace* i = cl->face;

			if (cl->null)
				continue;

			glm::vec3 lightVal = glm::vec3(0.f);
			float sunDot = glm::dot(sunDir, i->normal);
			rayHit r;
			if (sunDot > 0)
			{
				const int sampleCount = 64;
				float sunSpread = 0.017f;
				if (sampleCount == 1)
					sunSpread = 0;

				for (int samples = 0; samples < sampleCount; ++samples)
				{
					//r = RayTrace::rayTrace(cl->wPos + sunDir * .15f, sunDir + Util::randVec() * sunSpread, mesh);
					if(!fastRaytrace(Util::vec3Conv(cl->wPos + sunDir * .02f), 
						Util::vec3Conv(cl->wPos + (sunDir + Util::randVec() * sunSpread) * 1000.f),
						map->collisionState->world).hasHit())
					{
						lightVal += (glm::vec3(1.f, 0.95f, 0.9f) * glm::vec3((float)glm::max(sunDot, 0.f) * 4095.0f)) / (float)sampleCount;
					}
				}
			}
			for (auto& l : lights)
			{
				glm::vec3 dirToLight = glm::normalize(l.pos - cl->wPos);

				//dirToLight += Util::randVec() * .01f;

				float dot = glm::dot(dirToLight, i->normal);
				if (dot < 0)
					continue;

				float distanceToLightSquared = Util::distanceSquared(l.pos, cl->wPos);
				float diffuse = (glm::max(dot, 0.f) * l.intensity);

				auto rr = fastRaytrace(Util::vec3Conv(cl->wPos + dirToLight * .02f), 
					Util::vec3Conv(cl->wPos + dirToLight * 1000.0f), 
					map->collisionState->world);
				
				if(!rr.hasHit() || Util::distanceSquared(Util::vec3Conv(rr.m_hitPointWorld), cl->wPos) >= distanceToLightSquared)
				{
					if (l.type == LIGHT_SPOT)
						diffuse *= pow(glm::max(glm::dot(l.dir, -dirToLight), 0.0f), 4.f);

					lightVal += l.col * glm::vec3((float)(diffuse / (distanceToLightSquared) * 255.f));
				}
			}

			if (glm::length(lightVal) > 0.0f)
			{
				lightVal = glm::clamp(lightVal, 0.f, 255.f);
				setAt(x, y, lightMapRes, lightmap, lightVal);
				setAt(x + 1, y, lightMapRes, lightmap, lightVal * .1f);
				setAt(x - 1, y, lightMapRes, lightmap, lightVal * .1f);
				setAt(x, y + 1, lightMapRes, lightmap, lightVal * .1f);
				setAt(x, y - 1, lightMapRes, lightmap, lightVal * .1f);
			}
		}
	}
	std::cout << "Processing Lightmap, 100%    \r\n";

	/*
	for (float v = 0; v < 1.0f; v += lightMapStep)
	{
		std::cout << "Calculating bounces, " << (v * 100.0f) << "%\r";
		for (float u = 0; u < 1.0f; u += lightMapStep)
		{
			int x = u * lightMapRes;
			int y = v * lightMapRes;

			LightmapLumel* cl = &lightmapLumelXY[y * lightMapRes + x];
			LightmapFace* i = cl->face;

			if (cl->null)
				continue;

			glm::vec3 newCol = glm::vec3(0);

			for (auto& q : cl->visibleLumels)
			{
				if (q.lumel->null || i == nullptr) continue;
				float diffuse = (glm::max(glm::dot(q.dir, i->normal), 0.f)) * 150.f;
				newCol += getAt(q.lumel->uv.x * lightMapRes, q.lumel->uv.y * lightMapRes, lightMapRes, lightmap) / (q.distance * q.distance) * diffuse;
			}
			setAt(x, y, lightMapRes, lightmap, newCol);
		}
	}
	*/

	std::cout << std::endl;

	uint8_t* finalLightmap = new uint8_t[lightMapRes * lightMapRes * 3];
	for (int i = 0; i < lightMapRes * lightMapRes * 3; ++i)
	{
		finalLightmap[i] = glm::clamp(lightmap[i], 0, 255);
	}

	std::string lightmapName = map->fname + ".png";

	stbi_write_png(lightmapName.c_str(), lightMapRes, lightMapRes, 3, finalLightmap, lightMapRes * 3);

	mesh->hasLightmap = 1;
	mesh->lightmap = textureManager->loadTexture(lightmapName.c_str(), 1);

	delete lightmap;
}