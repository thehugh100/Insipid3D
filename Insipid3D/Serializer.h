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
	Serializer(uint8_t* val);
	Serializer(float* val);
	Serializer(std::string* val);
	Serializer(glm::vec3* val);
	Serializer(EntityTraits* val);
	Serializer(glm::mat4* val);

	void* getVal();
	std::string toString();
	void fromString(std::string in);

	enum types {SERIALIZE_INT, SERIALIZE_FLOAT, SERIALIZE_STRING, SERIALIZE_VEC3, SERIALIZE_ENT_TRAITS, SERIALIZE_BYTE, SERIALIZE_MAT4};
	int type;
	void* typeVal;
};

struct EntityVars
{
	std::string serialize();
	void registerVal(std::string name, Serializer val);
	std::map<std::string, Serializer> vals;
};