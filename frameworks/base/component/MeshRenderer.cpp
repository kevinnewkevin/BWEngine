#include "base/component/MeshRenderer.h"
#include "base/component/Camera.h"
#include "base/GameObject.h"
#include "renderer/GLProgramCache.h"
#include "renderer/Texture.h"
#include "base/ResourceManager.h"
#include "utils/FileUtils.h"
#include "renderer/Mesh.h"
#include "utils/MeshLoaderB3D.h"
#include "renderer/SkinnedMesh.h"
#include "renderer/Renderer.h"

MeshRenderer::MeshRenderer(GameObject* _gameObject/* = nullptr*/)
	: BaseComponent(_gameObject)
	, _texture(nullptr)
	, _mesh(nullptr)
	, _skinnedMesh(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
	SAFE_DELETE(_skinnedMesh);
	SAFE_DELETE(_mesh);
}

void MeshRenderer::setFile(const char* file)
{
	_filePath = FileUtils::getInstance()->fullPathForFilename(file);
	MeshLoaderB3D loader;
	if (loader.loadMesh(_filePath.c_str()))
		LOG("load mesh %s success", _filePath.c_str());

	_mesh = loader._meshVec[0];
	getSkin()->init(_mesh, loader.m_RootJoint, loader.m_TotalFrame);
}

void MeshRenderer::Awake()
{
}

void MeshRenderer::Start()
{
	_texture = ResourceManager::getInstance()->addTexture("nskinbl.tga");
	material = MeshMaterial::create();
	material->texture = _texture;
	material->positions = &_mesh->_positions;
	material->texCoords = &_mesh->_texCoords;
	material->indices = &_mesh->_indices;
}

void MeshRenderer::OnEnable()
{
}

void MeshRenderer::OnDisable()
{
}

void MeshRenderer::OnDestroy()
{
}

void MeshRenderer::Update(float dt)
{
	BaseComponent::Update(dt);
	if (_skinnedMesh)
	{
		_skinnedMesh->update(dt);
		material->positions = &_skinnedMesh->getSkinnedmeshPositions();
	}
}

void MeshRenderer::OnGUI()
{
	if (_mesh->_positions.size() <= 0)
		return;
	_cmd.init(material, &gameObject->mvpMatrix);
	Renderer::getInstance()->addCommand(&_cmd);
}

SkinnedMesh * MeshRenderer::getSkin()
{
	if (_skinnedMesh == nullptr) _skinnedMesh = new SkinnedMesh();
	return _skinnedMesh;
}
