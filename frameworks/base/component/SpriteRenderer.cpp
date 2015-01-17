#include "base/component/SpriteRenderer.h"
#include "base/GameObject.h"
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
	//_texture = ResourceManager::getInstance()->addTexture("E:/MyCode/BWEngine/tests/Resources/img2_2.png");
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

	float width = 153, height = 115;
	vec3 vertices[] = {
		vec3(0,  0, 0),
		vec3(width,   0, 0),
		vec3(0,   height, 0),
		vec3(width,  height, 0),
	};
	vec2 texCoords[] = { 
		vec2(0, 0),
		vec2(1, 0),
		vec2(0, 1),
		vec2(1, 1),
	};

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	unsigned short indices[] = { 0, 1, 2, 3 };
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, indices);
	
	CheckGLError();
	_texture->end();
}
