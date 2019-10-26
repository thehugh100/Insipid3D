#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "map.h"
#include "Light.h"
#include "TextureManager.h"

struct VisibleLumel;
struct LightmapFace;
struct LightmapLumel;

struct VisibleLumel
{
	VisibleLumel(LightmapLumel* lumel, float distance, glm::vec3 dir);
	LightmapLumel* lumel;
	float distance;
	glm::vec3 dir;
};

struct LightmapFace
{
	glm::vec3 normal;
	glm::vec3 tri[3];
	glm::vec2 uvs[3];
	float minX = 1;
	float maxX = 0;
	float minY = 1;
	float maxY = 0;
};

struct LightmapLumel
{
	glm::vec2 uv;
	glm::vec3 wPos;
	float bu, bv, bw;
	bool expandedLumel = 0;
	bool null = 0;
	LightmapFace* face;
	std::vector<VisibleLumel> visibleLumels;
};

struct LightmapGenerator
{
	inline bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 A, glm::vec3 B);
	inline bool PointInTriangle(glm::vec3 TriangleVectors[3], glm::vec3 P);
	inline bool inside_trigon(glm::vec2 s, glm::vec2 a, glm::vec2 b, glm::vec2 c);
	inline void barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float& u, float& v, float& w);
	inline void setAt(int x, int y, int lightMapRes, int* data, glm::vec3 col);
	glm::vec3 getAt(int x, int y, int lightMapRes, int* data);
	void buildLightmapData(Mesh* mesh, std::vector<LightmapFace>* lightmapFaces);
	void lightmapCalc(Map* map, int lightMapRes, std::vector<Light *> lights, TextureManager *textureManager);
};