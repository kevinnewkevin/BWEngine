#pragma once
#include "base/Ref.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "base/component/BaseComponent.h"
#include "base/Vector.h"

class Transform : public BaseComponent
{
public:
	Transform(GameObject* owner = nullptr);
	virtual ~Transform();

public:
	virtual void Update(float dt);
	Vector<Transform*>& getChildren();

public:
	Vec3& getPosition() { return _position; }
	void setPosition(const Vec3& pos);

	Vec3& getScale() { return _scale; }
	void setScale(const Vec3& scale);
	void setScale(float scale);

	Quat& getRotation() { return _rotation; }
	void setRotation(const Quat& rot);

	Vec3& getLocalPosition() { return _localPosition; }
	void setLocalPosition(const Vec3& pos);

	Vec3& getLocalScale() { return _localScale; }
	void setLocalScale(const Vec3& scale);

	Quat& getLocalRotation() { return _localRotation; }
	void setLocalRotation(const Quat& rot);

	Transform* getParent() { return _parent; }
	void setParent(Transform* parent);

	const Mat4& apply();
protected:
	Vec3 _position, _localPosition;
	Vec3 _scale, _localScale;
	Quat _rotation, _localRotation;
	Mat4 _modleMatrix;

	Transform* _parent;

	Vector<Transform*> _children;

protected:
	bool _transformDirty = true;
	bool _transformLocalDirty = false;
};

