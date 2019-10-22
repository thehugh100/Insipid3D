#include "Entity.h"
#include "engine.h"

Entity::Entity()
{
	entityTraits.setTrait("PureEntity");
}

Entity::Entity(Engine* engine)
	:engine(engine)
{
	entityTraits.setTrait("PureEntity");
}
