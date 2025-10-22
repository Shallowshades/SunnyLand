#include "component.h"

namespace engine::component {
void Component::setOwner(engine::object::GameObject* owner) {
	mOwner = owner;
}
engine::object::GameObject* Component::getOwner() const {
	return mOwner;
}
void Component::init()
{
}
void Component::handleInput()
{
}
void Component::update(float)
{
}
void Component::render()
{
}
void Component::clean()
{
}
} // namespace engine::component
