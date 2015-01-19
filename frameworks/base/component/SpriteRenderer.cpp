#include "base/component/SpriteRenderer.h"
#include "base/GameObject.h"
#include "renderer/GLProgramCache.h"
#include "renderer/Texture.h"
#include "base/ResourceManager.h"
#include "utils/FileUtils.h"

SpriteRenderer::SpriteRenderer(GameObject* _gameObject/* = nullptr*/)
	: BaseComponent(_gameObject)
{
}

SpriteRenderer::~SpriteRenderer()
{

}

void SpriteRenderer::Awake()
{
	vertices.assign({
		Vec3(), Vec3(),
		Vec3(), Vec3(),
	});
	texCoords.assign({
		Vec2(0, 0), Vec2(1, 0),
		Vec2(1, 1), Vec2(0, 1),
	});
	indices.assign({ 0, 1, 2, 0, 2, 3 });
}

void SpriteRenderer::Start()
{
	texture = ResourceManager::getInstance()->addTexture("img2_2.png");

	material = Material::create(GLProgramCache::getInstance()->getGLProgram(GLProgramCache::NAME::POSITION_TEXTURE));
	material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_POSITION, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
	material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, &texCoords[0]);
	material->setUniformTexture("Texture0", texture);

	_isDirty = true;
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
	updateTexture();
}

void SpriteRenderer::OnGUI()
{
	material->apply(gameObject->mvpMatrix);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &indices[0]);	
}

void SpriteRenderer::updateTexture()
{
	if (!_isDirty) return;
	_isDirty = false;

	float width = texture->getSize().width, height = texture->getSize().height;
	vertices[1].x = width;
	vertices[2].x = width, vertices[2].y = height;
	vertices[3].y = height;
}
