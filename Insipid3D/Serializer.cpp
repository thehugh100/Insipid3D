#include "Serializer.h"

Serializer::Serializer()
{
	typeVal = nullptr;
	type = SERIALIZE_INT;
}

Serializer::Serializer(int* val)
{
	typeVal = val;
	type = SERIALIZE_INT;
}

Serializer::Serializer(float* val)
{
	typeVal = val;
	type = SERIALIZE_FLOAT;
}

Serializer::Serializer(std::string* val)
{
	typeVal = val;
	type = SERIALIZE_STRING;
}

Serializer::Serializer(glm::vec3* val)
{
	typeVal = val;
	type = SERIALIZE_VEC3;
}

Serializer::Serializer(EntityTraits* val)
{
	typeVal = val;
	type = SERIALIZE_ENT_TRAITS;
}

void* Serializer::getVal()
{
	return typeVal;
}

void EntityVars::registerVal(std::string name, Serializer val)
{
	vals[name] = val;
}
