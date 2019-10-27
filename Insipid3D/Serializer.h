#pragma once
#include <iostream>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "EntityTraits.h"

struct Serializer
{
	Serializer();
	Serializer(int *val);
	Serializer(float* val);
	Serializer(std::string* val);
	Serializer(glm::vec3* val);
	Serializer(EntityTraits* val);

	void* getVal();

	enum types {SERIALIZE_INT, SERIALIZE_FLOAT, SERIALIZE_STRING, SERIALIZE_VEC3, SERIALIZE_ENT_TRAITS};
	int type;
	void* typeVal;
};

struct EntityVars
{
	void registerVal(std::string name, Serializer val);
	std::map<std::string, Serializer> vals;
};