#include "Entity.h"
#include "engine.h"

Entity::Entity()
{
	entityTraits.setTrait("PureEntity");
	active = 1;
}

Entity::Entity(Engine* engine)
	:engine(engine)
{
	entityTraits.setTrait("PureEntity");
	active = 1;
}
