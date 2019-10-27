#include "Entity.h"
#include "engine.h"
#include "Util.h"

Entity::Entity()
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("active", Serializer((int *)active));
	vars.registerVal("traits", Serializer(&entityTraits));
	vars.registerVal("type", Serializer(&entityType));
}

Entity::Entity(Engine* engine)
	:engine(engine)
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("active", Serializer((int*)active));
	vars.registerVal("type", Serializer(&entityType));
}

std::string Entity::serialize()
{
	nlohmann::json j;
	for (auto& i : vars.vals)
	{
		if (i.second.type == Serializer::SERIALIZE_INT)
			j[i.first] = reinterpret_cast<int>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_FLOAT)
			j[i.first] = *reinterpret_cast<float*>(i.second.typeVal);

		if (i.second.type == Serializer::SERIALIZE_STRING)
			j[i.first] = *reinterpret_cast<std::string *>(i.second.typeVal);
		
		if (i.second.type == Serializer::SERIALIZE_VEC3)
			j[i.first] = Util::printVec3(*reinterpret_cast<glm::vec3*>(i.second.typeVal));

		if (i.second.type == Serializer::SERIALIZE_ENT_TRAITS)
		{
			EntityTraits p = *reinterpret_cast<EntityTraits*>(i.second.typeVal);
			EntityTraits::EntityTraits_t *l = p.getTraits();
			nlohmann::json traits;
			for (auto& t : *l)
				traits += t;
			j[i.first] = traits;
		}
	}
	return j.dump();
}

