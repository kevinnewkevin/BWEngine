#include "base/component/BaseComponent.h"
#include "base/GameObject.h"


BaseComponent::BaseComponent(GameObject* _gameObject/* = nullptr*/)
	: gameObject(_gameObject)
{
}


BaseComponent::~BaseComponent()
{
	OnDestroy();
}

void BaseComponent::setEnabled(bool enable)
{
	if (_enable == enable) return;
	_enable = enable;
	if (_enable)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}

bool BaseComponent::isEnabled()
{
	return _enable;
}

bool BaseComponent::canDraw()
{
	return _canDraw && _enable;
}

void BaseComponent::Awake()
{
}

void BaseComponent::Start()
{
}

void BaseComponent::OnEnable()
{
}

void BaseComponent::OnDisable()
{
}

void BaseComponent::OnDestroy()
{
}

void BaseComponent::Update(float dt)
{
	if (_isStarted)
	{

	}
	else
	{
		Start();
		_isStarted = true;
	}
}

void BaseComponent::OnGUI()
{
}
