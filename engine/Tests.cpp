#include "Tests.h"
#include "DrawNode.h"
#include "Camera.h"
#include "Texture.h"
#include "Sprite2D.h"
#include "Sprite3D.h"
#include "BaseLight.h"

float sCameraMoveSpeed = 1.5f;

TestDrawPrimitive::TestDrawPrimitive()
{
	//camera
	Camera* camera = new Camera;
	//camera->Perspective(60, sWinW / sWinH, 0.1f, 1000.0);
	camera->Orthographic(1.0, 1.0, -1, 1);
	vec3 eye(0, 0, 0.5);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);
	DrawLines* l = new DrawLines;
	AddChild(l);
	l->DrawLine(vec3(-0.5, -0.5, 0), vec3(0.5, 0.5, 0), Color3B::RED, Color3B::RED);
	l->DrawLine(vec3(-0.5, 0.5, 0), vec3(0.5,  -0.5, 0), Color3B::GREEN, Color3B::GREEN);

	DrawPoints* p = new DrawPoints;
	AddChild(p);
	p->SetPointSize(10);
	p->DrawPoint(vec3(-0.25, 0, 0), Color3B::WHITE);
	p->DrawPoint(vec3(0.25, 0, 0), Color3B::WHITE);
	p->DrawPoint(vec3(0, 0.25, 0), Color3B::WHITE);
	p->DrawPoint(vec3(0, -0.25, 0), Color3B::WHITE);

	DrawTriangles* t = new DrawTriangles;
	AddChild(t);
	t->DrawTriangle(vec3(-0.1, -0.3, 0), vec3(0.1, -0.3, 0), vec3(0, 0.3, 0), Color3B::GREEN, Color3B::RED, Color3B::BLUE);
}

TestTextureSupport::TestTextureSupport()
{
	//camera
	Camera* camera = new Camera;
	//camera->Perspective(60, sWinW / sWinH, 0.1f, 1000.0);
	camera->Orthographic(1000, 1000.0 / sWinW * sWinH, -1, 1);
	vec3 eye(0, 0, 0.5);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	char buff[4][32] = { "img_test.bmp", "img_test.png", "img_test.dds", "img_cheryl.jpg" };
	for (int i = 0; i < 4; i++)
	{
		Texture2D* t = new Texture2D;
		t->LoadTextureFromImage(buff[i]);
		Sprite2D* sp = new Sprite2D;
		sp->InitWithTexture2D(t);
		sp->SetShader(shader_position_texture);
		AddChild(sp);
		sp->SetPositionX(-384 + i*256);
		sp->SetScale2D(vec2(0.9f));
	}
}

TestSprite3D::TestSprite3D()
{
	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 3000);
	vec3 eye(0, 500, 900);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("jeep_army.jpg");
	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("jeep.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood);
	mesh->SetColorTexture(texture);

	_sp = new Sprite3D;
	_sp->InitWithMesh(mesh);
	_sp->SetShader(shader_position_texture);
	AddChild(_sp);
}

void TestSprite3D::Step()
{
	static float ry = 0;
	ry += 1;
	_sp->SetRotation(vec3(0, ry, 0));
}

TestDirectionLight::TestDirectionLight()
{
	_diffuse = 0;
	_addDiffuse = true;

	//camera
	_camera = new Camera;
	_camera->Perspective(60, sWinW / sWinH, 0.1f, 100);
	vec3 eye(0, 7, 13);
	vec3 center(0, 0, 0);
	_camera->SetPosition(eye);
	_camera->LookAt(center);
	AddChild(_camera);

	//dir light
	_dirlight = new DirectionLight;
	_dirlight->SetColor(Color3F::WHITE);
	_dirlight->SetDirection(-eye);
	_dirlight->SetAmbientIntensity(0.1);
	_dirlight->SetDiffuseIntensity(_diffuse);
	AddChild(_dirlight);	
	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("shadowmap.DDS");
	texture->SetWrapType(eWrapType_reapeat);

	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("room_thickwalls.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal_smooth);
	mesh->SetColorTexture(texture);

	_sp = new Sprite3D;
	_sp->InitWithMesh(mesh);
	AddChild(_sp);
	_sp->SetRotation(vec3(0, 180, 0));
}

void TestDirectionLight::Step()
{
	if (_addDiffuse)
	{
		_diffuse += 0.01;
		if (_diffuse >= 1)
		{
			_addDiffuse = false;
			_diffuse = 1;
		}
	}
	else
	{
		_diffuse -= 0.01;
		if (_diffuse <= 0)
		{
			_addDiffuse = true;
			_diffuse = 0;
		}
	}
	_dirlight->SetDiffuseIntensity(_diffuse);

	_dirlight->SetDirection(-_camera->GetPosition());
}

TestPointLight::TestPointLight()
{
	_light1GoRight = true;
	vec3 light1Pos = vec3(-3, 1, 3);
	vec3 light2Pos = vec3(3, 5, -4);

	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 100);
	vec3 eye(0, 7, 13);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	_pointLight1 = new PointLight;
	_pointLight1->SetColor(Color3F::WHITE);
	_pointLight1->SetAmbientIntensity(0);
	_pointLight1->SetDiffuseIntensity(.5);
	_pointLight1->SetConstant(0.1f);
	_pointLight1->SetLinear(0.1f);
	_pointLight1->SetExp(0.5);
	_pointLight1->SetPosition(light1Pos);
	AddChild(_pointLight1);

	_pointLight2 = new PointLight;
	_pointLight2->SetColor(Color3F::WHITE);
	_pointLight2->SetAmbientIntensity(0);
	_pointLight2->SetDiffuseIntensity(.5);
	_pointLight2->SetConstant(0.1f);
	_pointLight2->SetLinear(0.1f);
	_pointLight2->SetExp(0.5);
	_pointLight2->SetPosition(light2Pos);
	AddChild(_pointLight2);

	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("shadowmap.DDS");
	texture->SetWrapType(eWrapType_reapeat);

	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("room_thickwalls.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal_smooth);
	mesh->SetColorTexture(texture);

	_sp = new Sprite3D;
	_sp->InitWithMesh(mesh);
	AddChild(_sp);
	_sp->SetRotation(vec3(0, 180, 0));
}

void TestPointLight::Step()
{
	vec3 light1pos = _pointLight1->GetPosition();
	if (_light1GoRight)
	{
		light1pos.x += 0.03f;
		if (light1pos.x >= 3)
		{
			light1pos.x = 3;
			_light1GoRight = false;
		}
	}
	else
	{
		light1pos.x -= 0.03f;
		if (light1pos.x <= -3)
		{
			light1pos.x = -3;
			_light1GoRight = true;
		}
	}
	_pointLight1->SetPosition(light1pos);
	_pointLight2->SetPosition(vec3(-light1pos.x, 5, -4));
}

TestSpotLight::TestSpotLight()
{
	_addY = true;
	_dirY = -1;

	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 100);
	vec3 eye(0, 7, 13);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	_spotLight = new SpotLight;
	_spotLight->SetColor(Color3F::WHITE);
	_spotLight->SetAmbientIntensity(0);
	_spotLight->SetDiffuseIntensity(0.3);
	_spotLight->SetConstant(1.0f);
	_spotLight->SetLinear(0.1f);
	_spotLight->SetExp(0);
	_spotLight->SetCutoff(0.8);
	_spotLight->SetDirection(vec3(0, _dirY, -1));
	_spotLight->SetPosition(vec3(0, 1, 1));
	AddChild(_spotLight);

	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("shadowmap.DDS");
	texture->SetWrapType(eWrapType_reapeat);

	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("room_thickwalls.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal_smooth);
	mesh->SetColorTexture(texture);

	_sp = new Sprite3D;
	_sp->InitWithMesh(mesh);
	AddChild(_sp);
	_sp->SetRotation(vec3(0, 180, 0));
}

void TestSpotLight::Step()
{
	if (_addY)
	{
		_dirY += 0.03f;
		if (_dirY >= 3)
		{
			_dirY = 3;
			_addY = false;
		}
	}
	else
	{
		_dirY -= 0.03f;
		if (_dirY <= -3)
		{
			_dirY = -3;
			_addY = true;
		}
	}

	_spotLight->SetDirection(vec3(0, _dirY, -1));
}

TestNormalMap::TestNormalMap()
{
	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 100);
	vec3 eye(0, 0, 3);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	DirectionLight* dirlight = new DirectionLight;
	dirlight->SetColor(Color3F::WHITE);
	dirlight->SetDirection(vec3(-1,-1,-3));
	dirlight->SetAmbientIntensity(0.3);
	dirlight->SetDiffuseIntensity(0.5);
	AddChild(dirlight);

	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("bricks.jpg");
	texture->SetWrapType(eWrapType_reapeat);

	_normal_texture = new Texture2D;
	_normal_texture->Retain();
	_normal_texture->LoadTextureFromImage("bricks_normal_map.jpg");
	_normal_texture->SetWrapType(eWrapType_reapeat);

	_un_normal_texture = new Texture2D;
	_un_normal_texture->Retain();
	_un_normal_texture->LoadTextureFromImage("bricks_normal_map_no.jpg");
	_un_normal_texture->SetWrapType(eWrapType_reapeat);

	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("box.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal | MeshAttribStep_tangent);
	mesh->SetNormalTexture(_normal_texture);
	mesh->SetColorTexture(texture);

	_sp = new Sprite3D;
	_sp->InitWithMesh(mesh);
	AddChild(_sp);
	_sp->SetPosition(vec3(0, 0, 0));
}

void TestNormalMap::Step()
{
	static float ry = 0;
	ry += 0.1;
	_sp->SetRotation(vec3(0, ry, 0));
	float xxx = sin(ry/3.0f);
	if (xxx > 0)
	{
		MeshManager::GetInstance()->GetMesh("box.obj")->SetNormalTexture(_normal_texture);
	}
	else
	{
		MeshManager::GetInstance()->GetMesh("box.obj")->SetNormalTexture(_un_normal_texture);
	}
}

TestCameraControl::TestCameraControl()
{
	sCameraMoveSpeed = 20;

	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 3000);
	vec3 eye(500, 500, 500);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);
	
	//line
	{
		DrawLines* l = new DrawLines;
		AddChild(l);
		l->DrawLine(vec3(0, 0, 0), vec3(1000, 0, 0), Color3B::RED, Color3B::RED);
		l->DrawLine(vec3(0, 0, 0), vec3(0, 1000, 0), Color3B::GREEN, Color3B::GREEN);
		l->DrawLine(vec3(0, 0, 0), vec3(0, 0, 1000), Color3B::BLUE, Color3B::BLUE);
	}

	Texture2D* texture = new Texture2D;
	texture->LoadTextureFromImage("jeep_army.jpg");
	Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("jeep.obj",
		MeshAttribStep_pos | MeshAttribStep_texcood);
	mesh->SetColorTexture(texture);

	Sprite3D* sp = new Sprite3D;
	sp->InitWithMesh(mesh);
	sp->SetShader(shader_position_texture);
	AddChild(sp);
}

TestCameraControl::~TestCameraControl()
{
	sCameraMoveSpeed = 1.5;
}

TestBillBoard::TestBillBoard()
{
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 100.0);
	vec3 eye(5, 0, 5);
	vec3 center(5, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);
	
	Texture2D* t = new Texture2D;
	t->LoadTextureFromImage("img_cheryl.jpg");

	Sprite2D* sp0 = new Sprite2D;
	sp0->InitWithTexture2D(t);
	sp0->SetShader(shader_position_texture);
	AddChild(sp0);
	sp0->SetPositionX(10.0/3.0);
	sp0->SetScale2D(vec2(0.005, 0.005));

	Sprite2D* sp1 = new Sprite2D;
	sp1->InitWithTexture2D(t);
	sp1->SetShader(shader_position_texture);
	sp1->EnableBillBoard(Sprite2D::eBillBoardType_plane);
	AddChild(sp1);
	sp1->SetPositionX(5);
	sp1->SetScale2D(vec2(0.005, 0.005));

	Sprite2D* sp2 = new Sprite2D;
	sp2->InitWithTexture2D(t);
	sp2->SetShader(shader_position_texture);
	sp2->EnableBillBoard(Sprite2D::eBillBoardType_rotate_y);
	AddChild(sp2);
	sp2->SetPositionX(10 - 10.0 / 3.0);
	sp2->SetScale2D(vec2(0.005, 0.005));
}

void TestBillBoard::Step()
{

}

TestSkeleton::TestSkeleton()
{
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 3000);
	vec3 eye(0, 1.5, 4);
	vec3 center(0, 1.5, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	_skelonMesh = MeshManager::GetInstance()->LoadMeshFromFile("boblampclean.md5mesh",
		MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_bone);
	_skelonMesh->GenTextures();

	Sprite3D* sp = new Sprite3D;
	sp->InitWithMesh(_skelonMesh);
	sp->SetShader(shader_position_texture);
	AddChild(sp);
	sp->SetScale(vec3(0.05, 0.05, 0.05));
	sp->SetRotation(vec3(-90, 0, 0));
}

void TestSkeleton::Step()
{
	static float r = 0;
	r += 0.03f;
	_skelonMesh->BoneTransform(r);
}

TestShadowMap::TestShadowMap()
{
	_lightX = -20;
	_lightXAdd = true;

	//line
	{
		DrawLines* l = new DrawLines;
		AddChild(l);
		l->DrawLine(vec3(0, 0, 0), vec3(1000, 0, 0), Color3B::RED, Color3B::RED);
		l->DrawLine(vec3(0, 0, 0), vec3(0, 1000, 0), Color3B::GREEN, Color3B::GREEN);
		l->DrawLine(vec3(0, 0, 0), vec3(0, 0, 1000), Color3B::BLUE, Color3B::BLUE);
	}

	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 100);
	vec3 eye(0, 7, 13);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	AddChild(camera);

	_dirlight = new DirectionLight;
	_dirlight->SetColor(Color3F::WHITE);
	_dirlight->SetDirection(vec3(_lightX, -10, -10));
	_dirlight->SetAmbientIntensity(0.2);
	_dirlight->SetDiffuseIntensity(0.9);
	AddChild(_dirlight);
	_dirlight->OpenShadow(true);
	
	{
		Texture2D* texture = new Texture2D;
		texture->LoadTextureFromImage("shadowmap.DDS");
		texture->SetWrapType(eWrapType_reapeat);

		Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("room_thickwalls.obj",
			MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal_smooth);
		mesh->SetColorTexture(texture);

		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(mesh);
		AddChild(sp);
		sp->SetRotation(vec3(0, 180, 0));
	}

	{
		Texture2D* texture = new Texture2D;
		texture->LoadTextureFromImage("bricks.jpg");
		texture->SetWrapType(eWrapType_reapeat);
		Texture2D* normal_texture = new Texture2D;
		normal_texture->LoadTextureFromImage("bricks_normal_map.jpg");
		normal_texture->SetWrapType(eWrapType_reapeat);

		Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("box.obj",
			MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal | MeshAttribStep_tangent);
		mesh->GenBuffers();
		mesh->SetNormalTexture(normal_texture);
		mesh->SetColorTexture(texture);

		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(mesh);
		AddChild(sp);
		sp->SetPosition(vec3(4, 3, 0));
	}
	
	{
		_skelonMesh = MeshManager::GetInstance()->LoadMeshFromFile("boblampclean.md5mesh",
			MeshAttribStep_pos | MeshAttribStep_texcood | MeshAttribStep_gen_normal_smooth | MeshAttribStep_bone);
		_skelonMesh->GenTextures();

		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(_skelonMesh);
		AddChild(sp);
		sp->SetScale(vec3(0.05, 0.05, 0.05));
		sp->SetRotation(vec3(-90, 0, 0));
		sp->SetPosition(vec3(-1,0,2));
	}
}

void TestShadowMap::Step()
{
	static float r = 0;
	r += 0.03f;
	_skelonMesh->BoneTransform(r);

	if (_lightXAdd)
	{
		_lightX += 0.1;
		if (_lightX >= 20)
		{
			_lightX = 20;
			_lightXAdd = false;
		}
	}
	else
	{
		_lightX -= 0.1;
		if (_lightX <= -20)
		{
			_lightX = -20;
			_lightXAdd = true;
		}
	}
	_dirlight->SetDirection(vec3(_lightX, -10, -10));
}

Tests::Tests()
{
	_tests.push_back(TestData("draw primitive test", [](){return new TestDrawPrimitive(); }));
	_tests.push_back(TestData("texture support test", [](){return new TestTextureSupport(); }));
	_tests.push_back(TestData("sprite3D test", [](){return new TestSprite3D(); }));
	_tests.push_back(TestData("skeleton test", [](){return new TestSkeleton(); }));
	_tests.push_back(TestData("camera control test", [](){return new TestCameraControl(); }));
	_tests.push_back(TestData("direction light test", [](){return new TestDirectionLight(); }));
	_tests.push_back(TestData("point light test", [](){return new TestPointLight(); }));
	_tests.push_back(TestData("spot light test", [](){return new TestSpotLight(); }));
	_tests.push_back(TestData("normal map test", [](){return new TestNormalMap(); }));
	_tests.push_back(TestData("bill board test", [](){return new TestBillBoard(); }));
	_tests.push_back(TestData("shadow map test", [](){return new TestShadowMap(); }));
}
