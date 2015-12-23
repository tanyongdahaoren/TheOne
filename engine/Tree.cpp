#include "Tree.h"
#include "Camera.h"
#include "Defines.h"

Tree::Tree()
{
	_currentCameraIdx = 0;
	//look at center of screen
	/*
	float zEye = sWinH / 1.1566f;

	Camera* camera = new Camera;

	camera->perspective(60, sWinW/sWinH, 0.1f, zEye + sWinH / 2.0f);

	vec3 eye(sWinW/2, sWinH/2.0f, zEye);
	vec3 center(sWinW/2, sWinH/2, 0.0f);
	
	camera->lookAt(eye, center);

	_cameras.push_back(camera);
	*/
}

void Tree::Travel(int idx)
{
	_directionLight = NULL;
	_pointLights.clear();
	_spotLights.clear();
	_cameras.clear();

	//transform all nodes on tree
	mat4 identity;
	VisitTransform(identity);

	//cameras view transform
	for (int i = 0; i < _cameras.size(); i++)
	{
		_cameras[i]->UpdateViewTransform();
	}

	//shadow pass
	if (_directionLight && _directionLight->IsOpenShadow())
	{
		_directionLight->BindRenderShadow();

		glClear(GL_DEPTH_BUFFER_BIT);

		_directionLight->CaculateVP();

		VisitRenderShadowMapping(_directionLight);

		_directionLight->UnBindRenderShadow();
	}

	if (idx == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	//render to screen
	for (int i = 0; i < _cameras.size(); i++)
	{
		_currentCameraIdx = i;
		VisitRender(_cameras[i]);
	}
}

void Tree::AddCamera(Camera* camera)
{
	_cameras.push_back(camera);
}

Camera* Tree::GetCurrentCamera()
{
	return _cameras[_currentCameraIdx];
}
