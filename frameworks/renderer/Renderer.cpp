#include "Renderer.h"
#include "renderer/GLProgramCache.h"

static Renderer* g_sInstance = nullptr;

Renderer * Renderer::getInstance()
{
	if (g_sInstance == nullptr) g_sInstance = new Renderer();
	return g_sInstance;
}

void Renderer::addCommand(RenderCommand * cmd)
{
	_queue.push(cmd);
}

void Renderer::render()
{
	while (!_queue.empty())
	{
		auto& cmd = _queue.front(); _queue.pop();
		switch (cmd->type)
		{
			case RenderCommand::TYPE::MESH:
			{
				auto material = (MeshMaterial*)cmd->material;
				material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_POSITION, 3, GL_FLOAT, GL_FALSE, 0, &material->positions->at(0));
				material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, &material->texCoords->at(0));
				material->setUniformTexture("Texture0", material->texture->getName());
				material->apply(*cmd->mat);
				glDrawElements(GL_TRIANGLES, material->indices->size(), GL_UNSIGNED_SHORT, &material->indices->at(0));
			}
				break;
			default:
				break;
		}
	}
}
