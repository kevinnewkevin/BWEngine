#pragma once
#include "base/Ref.h"

using std::vector;

class Mesh;
class Joint;

class SkinnedMesh 
{
public:
	SkinnedMesh() {}
	SkinnedMesh(Mesh* mesh, Joint* root, float totalFrame);
	~SkinnedMesh();
	
	void traverse(Joint* pJoint);
	void init();
	void init(Mesh* mesh, Joint* root, float totalFrame);
	void update(float fElapsedTime);
	void updateMesh(); 
	void updateVertex(Joint* joint, float frame);

	vector<Vec3>& getSkinnedmeshPositions();

	void setSpeed(float spd);
	float getSpeed();

	void addAnimation(std::string name, float startPos, float endPos);
	void removeAnimation(std::string name);
	void play(std::string name = "");
protected:
	Joint*			_rootJoint = nullptr;
	vector<Joint*>  _allJoint;
	Mesh*			_mesh = nullptr;

	// animation positions
	vector<Vec3>   _skinnedMeshPositions;

	std::map<std::string, std::tuple<float, float> > _animations;

	float	_startFrame = 0;
	float	_endFrame = 0;
	float	_currentFrame = 0;
	float   _totalFrame = 0;
	float	_speed = 1;
};