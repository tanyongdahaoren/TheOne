#pragma once

#include "List.h"
#include "Ref.h"
#include "MathH.h"

class Camera;
class Node : public Ref
{
public:
	typedef List<Node*>::iterator NodeIterator;
	Node();
	virtual ~Node();
	
	//-----------------------------
	//tree
	//-----------------------------
	void AddChild(Node* child);
	void RemoveChild(Node* child);
	void RemoveAllChildren();
	void RemoveFromParent();
	Node* GetParent();

	//-----------------------------
	//action
	//-----------------------------
	virtual void Stop();

	//-----------------------------
	//transform
	//-----------------------------
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);
	void SetPosition(vec3 pos);
	vec3 GetPosition();
	vec3 GetPositionInWorld();

	void SetRotation(vec3 rotation);
	void SetRotationQuat(quat rotationQuat);
	vec3 GetRotation();
	quat GetRotationQuat();

	void SetScaleX(float x);
	void SetScaleY(float y);
	void SetScaleZ(float z);
	void SetScale(vec3 scale);
	vec3 GetScale();
	
	const mat4& GetToParentTransform();
	mat4 GetToWorldTransform();
protected:
	//-----------------------------
	//render
	//-----------------------------
	virtual void Visit(const mat4& parentToWorldTransform);
	virtual void Draw(Camera* camera);

	//-----------------------------
	//transform
	//-----------------------------
	void UpdateRotation();
	void UpdateRotationQuat();
protected:
	//-----------------------------
	//tree
	//-----------------------------
	Node* _parent;
	List<Node*> _children;
	NodeIterator _it;

	//-----------------------------
	//transform
	//-----------------------------
	vec3 _position;

	float _rotationX;
	float _rotationY;
	float _rotationZ;
	quat _rotationQuat;
	
	float _scaleX;
	float _scaleY;
	float _scaleZ;

	mat4 _toWorldTransform;
	mat4 _toParentTransform;
	bool _transformDirty;
};
