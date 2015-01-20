#include "SkinnedMesh.h"
#include "Joint.h"
#include "Mesh.h"
#include "base/macros/BWMacros.h"
#include "utils/MathUtils.h"

SkinnedMesh::SkinnedMesh(Mesh* mesh, Joint* root, float totalFrame)
						:_mesh(mesh)
						, _rootJoint(root)
						, _totalFrame(totalFrame)
						, _currentFrame(0)

{
	init();
}

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::traverse(Joint* joint)
{
	if (joint == NULL)
		return;

	_allJoint.push_back(joint);
	for (unsigned int i = 0; i < joint->children.size(); ++i)
	{
		traverse(joint->children[i]);
	}

	_skinnedMeshPositions.assign(_mesh->_positions.begin(), _mesh->_positions.end());
}

void SkinnedMesh::init()
{
	traverse(_rootJoint);

	for (unsigned int i = 0; i < _allJoint.size(); i++)
	{
		Joint* joint = _allJoint.at(i);
		Mat4 positionMatrix;
		positionMatrix.buildTranslate(joint->position);
		Mat4 rotationMatrix;
		rotationMatrix = joint->rotation.toMatrix();
		Mat4 LocalAniMatrix = positionMatrix*rotationMatrix;

		if (joint->parent)
		{
			joint->globalAnimatedMatrix = joint->parent->globalAnimatedMatrix*LocalAniMatrix;
		}
		else
		{
			joint->globalAnimatedMatrix = LocalAniMatrix;
		}

		joint->globalInvMatrix = joint->globalAnimatedMatrix.inverse();
	}

}

void SkinnedMesh::init(Mesh * mesh, Joint * root, float totalFrame)
{
	_mesh = mesh;
	_rootJoint = root;
	_totalFrame = totalFrame;
	_currentFrame = 0;
	init();
}

void SkinnedMesh::updateVertex(Joint* joint, float frame)
{
	if (joint)
	{
		Vec3 position;
		Quat rotation;
		joint->getFrame(frame, position, rotation);

		Mat4 mat;
		if (joint->parent)
		{
			mat = joint->parent->globalAnimatedMatrix;
			joint->position = position;
		}

		Mat4 positionMatrix;
		positionMatrix.buildTranslate(position);
		Mat4 rotationMatrix;
		rotationMatrix = rotation.toMatrix();
		Mat4 localAniMatrix = positionMatrix * rotationMatrix;

		if (joint->parent)
			joint->globalAnimatedMatrix = joint->parent->globalAnimatedMatrix * localAniMatrix;
		else
			joint->globalAnimatedMatrix = localAniMatrix;

		mat = joint->globalAnimatedMatrix * joint->globalInvMatrix;
		
		for (unsigned int i = 0; i < joint->vertexIndices.size(); ++i)
		{
			int vertex = joint->vertexIndices[i];
			Vec3 p = _mesh->_positions[vertex];
			mat.transformVec3(p.x, p.y, p.z);
			_skinnedMeshPositions[vertex] = p * joint->vertexWeights[i];
		}
	}
}

void SkinnedMesh::updateMesh()
{
	for (unsigned int i = 0; i < _allJoint.size(); i++)
	{
		updateVertex(_allJoint.at(i), _currentFrame);
	}
}

void SkinnedMesh::update(float fElapsedTime)
{
	if (_mesh == nullptr) return;
	_currentFrame += fElapsedTime * _speed;

	if (_currentFrame > _endFrame)
		_currentFrame = _startFrame;

	updateMesh();
}

vector<Vec3>& SkinnedMesh::getSkinnedmeshPositions()
{
	return _skinnedMeshPositions;
}

void SkinnedMesh::setSpeed(float spd)
{
	_speed = spd;
}

float SkinnedMesh::getSpeed()
{
	return _speed;
}

void SkinnedMesh::addAnimation(std::string name, float startPos, float endPos)
{
	if (startPos > endPos) std::swap(startPos, endPos);
	_animations[name] = std::make_tuple(startPos, endPos);
}

void SkinnedMesh::removeAnimation(std::string name)
{
	_animations.erase(name);
}

void SkinnedMesh::play(std::string name)
{
	float start, end;
	if (_animations.count(name) != 0)
	{
		auto& param = _animations[name];
		start = std::get<0>(param);
		end = std::get<1>(param);
		start = clampf(start, 0, _totalFrame);
		end = clampf(end, 0, _totalFrame);
	}
	else
	{
		start = 0;
		end = _totalFrame;
	}
	_startFrame = start;
	_endFrame = end;
	_currentFrame = start;
}
