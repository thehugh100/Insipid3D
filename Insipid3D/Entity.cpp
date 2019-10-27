#include "Entity.h"
#include "engine.h"
#include "Util.h"

Entity::Entity()
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("active", Serializer(&active));
	vars.registerVal("traits", Serializer(&entityTraits));
	vars.registerVal("type", Serializer(&entityType));
}

Entity::Entity(Engine* engine)
	:engine(engine)
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("active", Serializer(&active));
	vars.registerVal("type", Serializer(&entityType));
}

std::string Entity::serialize()
{
	return vars.serialize();
}

