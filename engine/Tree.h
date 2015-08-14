#pragma once

#include "Node.h"
#include "Vector.h"
#include "BaseLight.h"

class Camera;
class BaseLight;
class Tree : public Node
{
public:
	Tree();
		
	void Travel();

	void AddCamera(Camera* camera);

	void SetDirectionLight(DirectionLight* light){ _directionLight = light; }

	void AddPointLight(PointLight* light) { _pointLights.push_back(light); }

	Camera* GetCurrentCamera();
public:
	Vector<Camera*> _cameras;

	DirectionLight* _directionLight;

	vector<PointLight*> _pointLights;
};