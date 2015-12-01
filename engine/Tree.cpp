#include "Tree.h"
#include "Camera.h"
#include "Defines.h"

static Camera* sCurrentCamera = NULL;

Tree::Tree()
{
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

void Tree::Travel()
{
	_directionLight = NULL;
	_pointLights.clear();
	_spotLights.clear();
	_cameras.clear();

	//transform all nodes on tree
	mat4 identity;
	VisitTransform(identity);

	//shadow pass
	if (_directionLight && _directionLight->IsOpenShadow())
	{
		_directionLight->BindRenderShadow();

		glClear(GL_DEPTH_BUFFER_BIT);

		_directionLight->CaculateVP();

		VisitRenderShadowMapping(_directionLight->GetVP());

		_directionLight->UnBindRenderShadow();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render to screen
	for ( const auto &camera: _cameras )
	{
		sCurrentCamera = camera;

		VisitRender(camera);
	}
}

void Tree::AddCamera(Camera* camera)
{
	_cameras.push_back(camera);
}

Camera* Tree::GetCurrentCamera()
{
	return sCurrentCamera;
}
