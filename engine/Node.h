#pragma once

#include "List.h"
#include "Ref.h"
#include "MathH.h"
#include "Types.h"

class Camera;
class Node : public Ref
{
	friend class Sprite2D;
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
	const List<Node*>& GetChildren();

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

	virtual void SetScaleX(float x);
	virtual void SetScaleY(float y);
	virtual void SetScaleZ(float z);
	virtual void SetScale(vec3 scale);
	virtual vec3 GetScale();
	
	Size3D GetModelSize();

	void SetAnchorPoint(vec3 p);
	vec3 GetAnchorPoint();

	const mat4& GetToParentTransform();
	mat4 GetToWorldTransform();
protected:
	void SetModelSize(Size3D size);

	//-----------------------------
	//render
	//-----------------------------
	virtual void VisitTransform(const mat4& parentToWorldTransform);
	virtual void VisitRender(const mat4& cameraProjTransform, const mat4& cameraViewTransform);
	virtual void Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform){}
	virtual void VisitRenderShadowMapping(const mat4& lightProjTransform, const mat4& lightViewTransform);
	virtual void RenderShadowMapping(const mat4& lightProjTransform, const mat4& lightViewTransform){}
	virtual void UpdateWorldTransorm(const mat4& parentToWorldTransform);

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

	Size3D _modelSize;

	vec3 _anchorPoint;
};
