#include "Entity.h"
#include "engine.h"
#include "Util.h"

Entity::Entity()
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("id", Serializer(&id));
	vars.registerVal("active", Serializer(&active));
	vars.registerVal("traits", Serializer(&entityTraits));
	vars.registerVal("type", Serializer(&entityType));
	initialised = 0;
}

Entity::Entity(Engine* engine)
	:engine(engine)
{
	entityType = "PureEntity";
	entityTraits.setTrait("PureEntity");
	active = 1;
	vars.registerVal("active", Serializer(&active));
	vars.registerVal("type", Serializer(&entityType));
	vars.registerVal("id", Serializer(&id));
	initialised = 0;
}

std::string Entity::serialize()
{
	return vars.serialize();
}

