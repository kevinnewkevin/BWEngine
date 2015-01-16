#include "base/component/BaseComponent.h"
#include "base/GameObject.h"


BaseComponent::BaseComponent(GameObject* _gameObject/* = nullptr*/)
	: gameObject(_gameObject)
{
}


BaseComponent::~BaseComponent()
{
}
