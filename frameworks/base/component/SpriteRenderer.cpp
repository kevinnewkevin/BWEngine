#include "base/component/SpriteRenderer.h"
#include "base/GameObject.h"
#include "renderer/GLProgram.h"
#include "renderer/Texture.h"
#include "base/ResourceManager.h"

SpriteRenderer::SpriteRenderer(GameObject* _gameObject/* = nullptr*/)
	: BaseComponent(_gameObject)
	,_texture(nullptr)
{
}

SpriteRenderer::~SpriteRenderer()
{

}

void SpriteRenderer::Awake()
{
}

void SpriteRenderer::Start()
{
	_texture = ResourceManager::getInstance()->addTexture("img2_2.png");

	glEnable(GL_TEXTURE_2D);
}

void SpriteRenderer::OnEnable()
{
}

void SpriteRenderer::OnDisable()
{
}

void SpriteRenderer::OnDestroy()
{
}

void SpriteRenderer::Update(float dt)
{
	BaseComponent::Update(dt);
}

void SpriteRenderer::OnGUI()
{
	_texture->begin();

	float width = _texture->getSize().width, height = _texture->getSize().height;
	Vec3 vertices[] = {
		Vec3(0,		0,		0),
		Vec3(width, 0,		0),
		Vec3(width, height, 0),
		Vec3(0,		height, 0),
	};
	Vec2 texCoords[] = { 
		Vec2(0, 0),
		Vec2(1, 0),
		Vec2(1, 1),
		Vec2(0, 1),
	};

	unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };
	glEnableVertexAttribArray(GLProgram::ATTRIBUTE::VERTEX_POSITION);
	glVertexAttribPointer(GLProgram::ATTRIBUTE::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, vertices);

	glEnableVertexAttribArray(GLProgram::ATTRIBUTE::VERTEX_TEX_COORD);
	glVertexAttribPointer(GLProgram::ATTRIBUTE::VERTEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	
	CheckGLError();
	_texture->end();
}
