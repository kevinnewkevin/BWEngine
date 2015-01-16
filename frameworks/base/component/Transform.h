#pragma once
#include "base/Ref.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "base/component/BaseComponent.h"

class Transform : public BaseComponent
{
public:
	Transform(GameObject* owner = nullptr);
	virtual ~Transform();

public:
	Vec3& getPosition() { return _position; }
	void setPosition(const Vec3& pos);

	Vec3& getScale() { return _scale; }
	void setScale(const Vec3& scale);

	Vec3& getRotation() { return _rotation; }
	void setRotation(const Vec3& rot);

	Vec3& getLocalPosition() { return _localPosition; }
	void setLocalPosition(const Vec3& pos);

	Vec3& getLocalScale() { return _localScale; }
	void setLocalScale(const Vec3& scale);

	Vec3& getLocalRotation() { return _localRotation; }
	void setLocalRotation(const Vec3& rot);

	Transform* getParent() { return _parent; }
	void setParent(Transform* parent);

	const Mat4& apply();
protected:
	Vec3 _position, _localPosition;
	Vec3 _scale, _localScale;
	Vec3 _rotation, _localRotation;
	Mat4 _modleMatrix;

	Transform* _parent;

	std::vector<Transform*> _children;

protected:
	bool _transformDirty = true;
	bool _transformLocalDirty = false;
};

