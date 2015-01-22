#include "base/component/SpriteRenderer.h"
#include "base/GameObject.h"
#include "renderer/GLProgramCache.h"
#include "renderer/Texture.h"
#include "renderer/Renderer.h"
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
	_mesh._positions.assign({
		Vec3(), Vec3(),
		Vec3(), Vec3(),
	});
	_mesh._texCoords.assign({
		Vec2(0, 0), Vec2(1, 0),
		Vec2(1, 1), Vec2(0, 1),
	});
	_mesh._indices.assign({ 0, 1, 2, 0, 2, 3 });
}

void SpriteRenderer::Start()
{
	//texture = ResourceManager::getInstance()->addTexture("img2_2.png");
	texture = ResourceManager::getInstance()->addTexture("BigGlow3.bmp");

	material = MeshMaterial::create();
	material->texture = texture;
	material->positions = &_mesh._positions;
	material->texCoords = &_mesh._texCoords;
	material->indices = &_mesh._indices;

	_textureRect = Rect(Vec2(0, 0), texture->getSize());
	_isDirty = true;

	_textureRect = Rect(Vec2(30, 30), Size(50, 50));
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
	_cmd.init(material, &gameObject->mvpMatrix);
	Renderer::getInstance()->addCommand(&_cmd);	
}

void SpriteRenderer::updateTexture()
{
	if (!_isDirty) return;
	_isDirty = false;

	float origionWidth = texture->getSize().width;
	float origionHeight = texture->getSize().height;

	float x = _textureRect.x, y = _textureRect.y;
	float width = _textureRect.width, height = _textureRect.height;
	_mesh._positions[0].x = x,			_mesh._positions[0].y = y;
	_mesh._positions[1].x = x + width,	_mesh._positions[1].y = y;
	_mesh._positions[2].x = x + width,	_mesh._positions[2].y = y + height;
	_mesh._positions[3].x = 0,			_mesh._positions[3].y = y + height;

	_mesh._texCoords[0].x = _mesh._positions[0].x / origionWidth, _mesh._texCoords[0].y = _mesh._positions[0].y / origionHeight;
	_mesh._texCoords[1].x = _mesh._positions[1].x / origionWidth, _mesh._texCoords[1].y = _mesh._positions[1].y / origionHeight;
	_mesh._texCoords[2].x = _mesh._positions[2].x / origionWidth, _mesh._texCoords[2].y = _mesh._positions[2].y / origionHeight;
	_mesh._texCoords[3].x = _mesh._positions[3].x / origionWidth, _mesh._texCoords[3].y = _mesh._positions[3].y / origionHeight;
}

void SpriteRenderer::setTextureRect(Rect & rect)
{
	_textureRect = rect;
}
