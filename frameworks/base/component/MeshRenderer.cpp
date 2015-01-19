#include "base/component/MeshRenderer.h"
#include "base/component/Camera.h"
#include "base/GameObject.h"
#include "renderer/GLProgramCache.h"
#include "renderer/Texture.h"
#include "base/ResourceManager.h"
#include "utils/FileUtils.h"
#include "renderer/Mesh.h"
#include "utils/MeshLoaderB3D.h"

MeshRenderer::MeshRenderer(GameObject* _gameObject/* = nullptr*/)
	: BaseComponent(_gameObject)
	, _texture(nullptr)
	, _mesh(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
	SAFE_DELETE(_mesh);
}

void MeshRenderer::setFile(const char* file)
{
	_filePath = FileUtils::getInstance()->fullPathForFilename(file);
}

void MeshRenderer::Awake()
{
	MeshLoaderB3D loader;
	if (loader.loadMesh(_filePath.c_str()))
		LOG("load mesh %s success", _filePath.c_str());

	_mesh = loader._meshVec[0];
}

void MeshRenderer::Start()
{
	_texture = ResourceManager::getInstance()->addTexture("nskinbl.jpg");

	material = Material::create(GLProgramCache::getInstance()->getGLProgram(GLProgramCache::NAME::POSITION_TEXTURE));
	material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_POSITION, 3, GL_FLOAT, GL_FALSE, 0, &_mesh->_positions[0]);
	material->setVertexAttribPointer(GLProgram::ATTRIBUTE::NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, &_mesh->_texCoords[0]);
	material->setUniformTexture("Texture0", _texture);
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
}

void MeshRenderer::OnGUI()
{
	if (_mesh->_positions.size() <= 0)
		return;

	material->apply(gameObject->mvpMatrix);
	glDrawElements(GL_TRIANGLES, _mesh->_indices.size(), GL_UNSIGNED_SHORT, &_mesh->_indices[0]);
	
	CheckGLError();
}
