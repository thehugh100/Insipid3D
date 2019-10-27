#include "Serializer.h"
#include "Util.h"
#include <nlohmann/json.hpp>

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

Serializer::Serializer(uint8_t* val)
{
	typeVal = val;
	type = SERIALIZE_BYTE;
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

Serializer::Serializer(glm::mat4* val)
{
	typeVal = val;
	type = SERIALIZE_MAT4;
}

void* Serializer::getVal()
{
	return typeVal;
}

std::string Serializer::toString()
{
	if (type == Serializer::SERIALIZE_INT)
		return std::to_string(*reinterpret_cast<int*>(typeVal));

	if (type == Serializer::SERIALIZE_FLOAT)
		return std::to_string(*reinterpret_cast<float*>(typeVal));

	if (type == Serializer::SERIALIZE_BYTE)
		return std::to_string(*reinterpret_cast<uint8_t*>(typeVal));

	if (type == Serializer::SERIALIZE_STRING)
		return *reinterpret_cast<std::string*>(typeVal);

	if (type == Serializer::SERIALIZE_VEC3)
		return Util::printVec3(*reinterpret_cast<glm::vec3*>(typeVal));

	if (type == Serializer::SERIALIZE_MAT4)
		return Util::printMat4(*reinterpret_cast<glm::mat4*>(typeVal));

	if (type == Serializer::SERIALIZE_ENT_TRAITS)
	{
		EntityTraits p = *reinterpret_cast<EntityTraits*>(typeVal);
		EntityTraits::EntityTraits_t* l = p.getTraits();
		nlohmann::json traits;
		for (auto& t : *l)
			traits.push_back(t);
		return traits.dump();
	}

	return "No serialization for type.";
}

void Serializer::fromString(std::string in)
{
	if (type == Serializer::SERIALIZE_INT)
		*reinterpret_cast<int*>(typeVal) = atoi(in.c_str());

	if (type == Serializer::SERIALIZE_FLOAT)
		*reinterpret_cast<float*>(typeVal) = atof(in.c_str());

	if (type == Serializer::SERIALIZE_BYTE)
		*reinterpret_cast<uint8_t*>(typeVal) = atoi(in.c_str());

	if (type == Serializer::SERIALIZE_STRING)
		*reinterpret_cast<std::string*>(typeVal) = in;

	if (type == Serializer::SERIALIZE_VEC3)
		*reinterpret_cast<glm::vec3*>(typeVal) = Util::vec3FromString(in);

	if (type == Serializer::SERIALIZE_MAT4)
		*reinterpret_cast<glm::mat4*>(typeVal) = Util::mat4FromString(in);
}

std::string EntityVars::serialize()
{
	nlohmann::json j;
	for (auto& i : vals)
	{
		if (i.second.type == Serializer::SERIALIZE_INT)
			j[i.first] = *reinterpret_cast<int*>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_FLOAT)
			j[i.first] = *reinterpret_cast<float*>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_BYTE)
			j[i.first] = *reinterpret_cast<uint8_t*>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_STRING)
			j[i.first] = *reinterpret_cast<std::string*>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_VEC3)
			j[i.first] = Util::printVec3(*reinterpret_cast<glm::vec3*>(i.second.typeVal));

		if (i.second.type == Serializer::SERIALIZE_MAT4)
			j[i.first] = Util::printMat4(*reinterpret_cast<glm::mat4*>(i.second.typeVal));

		if (i.second.type == Serializer::SERIALIZE_ENT_TRAITS)
		{
			EntityTraits p = *reinterpret_cast<EntityTraits*>(i.second.typeVal);
			EntityTraits::EntityTraits_t* l = p.getTraits();
			nlohmann::json traits;
			for (auto& t : *l)
				traits += t;
			j[i.first] = traits;
		}
	}
	return j.dump();
}

void EntityVars::registerVal(std::string name, Serializer val)
{
	vals[name] = val;
}
