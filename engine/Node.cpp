#include "Node.h"
#include "Director.h"
#include "Tree.h"
#include "Camera.h"

Node::Node()
	: _parent(NULL)
	, _rotationX(0)
	, _rotationY(0)
	, _rotationZ(0)
	, _scaleX(1)
	, _scaleY(1)
	, _scaleZ(1)
	, _transformDirty(false)
	, _anchorPoint(0.5,0.5,0.5)
{
}

Node::~Node()
{
}

void Node::AddChild(Node* child)
{
	NodeIterator it = _children.push_back(child);
	child->_it = it;
	child->_parent = this;
}

void Node::RemoveFromParent()
{
	if (_parent)
	{
		_parent->RemoveChild(this);
	}
}

void Node::RemoveChild(Node* child)
{
	child->_parent = NULL;
	child->Stop();

	_children.erase(child->_it);
}

void Node::RemoveAllChildren()
{
	for (auto it = _children.begin(); it!=_children.end(); ++it)
	{
		(*it)->_parent = NULL;
		(*it)->Stop();
	}

	_children.clear();
}

Node* Node::GetParent()
{
	return _parent;
}


const List<Node*>& Node::GetChildren()
{
	return _children;
}

void Node::Stop()
{
	for( const auto &child: _children)
		child->Stop();
}

void Node::SetPositionX(float x)
{
	_position.x = x;
	_transformDirty = true;
}

void Node::SetPositionY(float y)
{
	_position.y = y;
	_transformDirty = true;
}

void Node::SetPositionZ(float z)
{
	_position.z = z;
	_transformDirty = true;
}

void Node::SetPosition(vec3 pos)
{
	_position = pos;
	_transformDirty = true;
}

glm::vec3 Node::GetPosition()
{
	return _position;
}

glm::vec3 Node::GetPositionInWorld()
{
	vec4 worldPos = vec4(_position.x, _position.y, _position.z, 1) * GetToWorldTransform();
	return vec3(worldPos.x, worldPos.y, worldPos.z);
}

void Node::SetRotation(vec3 rotation)
{
	_rotationX = rotation.x;
	_rotationY = rotation.y;
	_rotationZ = rotation.z;
	UpdateRotationQuat();
	_transformDirty = true;
}

void Node::SetRotationQuat(quat rotationQuat)
{
	_rotationQuat = rotationQuat;
	UpdateRotation();
	_transformDirty = true;
}

glm::vec3 Node::GetRotation()
{
	return vec3(_rotationX, _rotationY, _rotationZ);
}

glm::quat Node::GetRotationQuat()
{
	return _rotationQuat;
}

void Node::SetScaleX(float x)
{
	_scaleX = x;
	_transformDirty = true;
}

void Node::SetScaleY(float y)
{
	_scaleY = y;
	_transformDirty = true;
}

void Node::SetScaleZ(float z)
{
	_scaleZ = z;
	_transformDirty = true;
}

void Node::SetScale(vec3 scale)
{
	_scaleX = scale.x;
	_scaleY = scale.y;
	_scaleZ = scale.z;
	_transformDirty = true;
}

glm::vec3 Node::GetScale()
{
	return vec3(_scaleX, _scaleY, _scaleZ);
}

void Node::SetModelSize(Size3D size)
{
	_modelSize = size;
	_transformDirty = true;
}

Size3D Node::GetModelSize()
{
	return _modelSize;
}

void Node::SetAnchorPoint(vec3 p)
{
	_anchorPoint = p;
	_transformDirty = true;
}

glm::vec3 Node::GetAnchorPoint()
{
	return _anchorPoint;
}

void Node::UpdateRotation()
{
	_rotationX = pitch(_rotationQuat);
	_rotationY = yaw(_rotationQuat);
	_rotationZ = roll(_rotationQuat);
}

void Node::UpdateRotationQuat()
{
	_rotationQuat = quat(vec3(glm::radians(_rotationX), glm::radians(_rotationY), glm::radians(_rotationZ)));
}

const mat4& Node::GetToParentTransform()
{
	if (!_transformDirty)
	{
		return _toParentTransform;
	}
	mat4 s;
	s = glm::scale(s, vec3(_scaleX, _scaleY, _scaleZ));

	mat4 r = glm::mat4_cast(_rotationQuat);

	mat4 offsetMat;
	
	if (_modelSize.w!=0 && _modelSize.h!=0)
	{
		vec3 offset;
		vec3 zeroAnchorPointOffset = vec3(0.5f, 0.5f, 0.5f);
		vec3 currentAnchorPointOffset = zeroAnchorPointOffset - _anchorPoint;
		offset = vec3(
			_modelSize.w * currentAnchorPointOffset.x,
			_modelSize.h * currentAnchorPointOffset.y,
			_modelSize.d * currentAnchorPointOffset.z);

		offsetMat = glm::translate(offsetMat, offset);
	}
	
	mat4 t;
	t = glm::translate(t, _position);

	_toParentTransform = t * r * s * offsetMat;

	_transformDirty = false;

	return _toParentTransform;
}

glm::mat4 Node::GetToWorldTransform()
{
	mat4 m = GetToParentTransform();
	Node* parent = _parent;
	while (parent)
	{
		m = parent->GetToParentTransform() * m;
		parent = parent->GetParent();
	}
	return m;
}

void Node::UpdateWorldTransorm(const mat4& parentToWorldTransform)
{
	_toWorldTransform = parentToWorldTransform * GetToParentTransform();
}

void Node::VisitTransform(const mat4& parentToWorldTransform)
{
	UpdateWorldTransorm(parentToWorldTransform);

	if (dynamic_cast<SpotLight*>(this))
	{
		Director::GetInstance()->GetCurrentTree()->AddSpotLight((SpotLight*)this);
	}
	else if (dynamic_cast<PointLight*>(this))
	{
		Director::GetInstance()->GetCurrentTree()->AddPointLight((PointLight*)this);
	}
	else if (dynamic_cast<DirectionLight*>(this))
	{
		Director::GetInstance()->GetCurrentTree()->SetDirectionLight((DirectionLight*)this);
	}
	else if (dynamic_cast<Camera*>(this))
	{
		Director::GetInstance()->GetCurrentTree()->AddCamera(((Camera*)this));
	}

	for ( const auto &child: _children )
	{
		child->VisitTransform(_toWorldTransform);
	}
}

void Node::VisitRender(Camera* camera)
{
	Render(camera);

	for (const auto &child : _children)
	{
		child->VisitRender(camera);
	}
}

void Node::VisitRenderShadowMapping(Camera* camera)
{
	RenderShadowMapping(camera);

	for (const auto &child : _children)
	{
		child->VisitRenderShadowMapping(camera);
	}
}
