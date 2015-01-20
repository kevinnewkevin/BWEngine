#pragma once
#include "base/Ref.h"
#include "renderer/command/RenderCommand.h"
#include "renderer/command/MeshCommand.h"

class Renderer : public Ref
{
public:
	static Renderer* getInstance();

public:
	Renderer() {}
	virtual ~Renderer() {}
	
	void addCommand(RenderCommand* cmd);

	void render();

protected:
	std::queue<RenderCommand*> _queue;
};