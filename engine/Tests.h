#pragma once

#include "Node.h"
#include <functional>
#include <vector>

using namespace std;

class Texture2D;
class Camera;
class Sprite3D;
class DirectionLight;
class PointLight;
class SpotLight;
class Mesh;
extern float sCameraMoveSpeed;

class TestBase : public Node
{
public:
	virtual void Step() {};
};

class TestDrawPrimitive : public TestBase
{
public:
	TestDrawPrimitive();
	virtual void Step() {};
};

class TestTextureSupport : public TestBase
{
public:
	TestTextureSupport();
	virtual void Step() {};
};

class TestSprite3D : public TestBase
{
public:
	TestSprite3D();
	virtual void Step();
	Sprite3D* _sp;
};

class TestSkeleton : public TestBase
{
public:
	TestSkeleton();
	virtual void Step();
	Sprite3D* _sp;
	Mesh* _skelonMesh;
};

class TestCameraControl : public TestBase
{
public:
	TestCameraControl();
	~TestCameraControl();
	virtual void Step(){}
};

class TestDirectionLight : public TestBase
{
public:
	TestDirectionLight();
	virtual void Step();
	Sprite3D* _sp;
	DirectionLight* _dirlight;
	float _diffuse;
	bool _addDiffuse;
	Camera* _camera;
};

class TestPointLight : public TestBase
{
public:
	TestPointLight();
	virtual void Step();
	Sprite3D* _sp;
	PointLight* _pointLight1;
	PointLight* _pointLight2;
	bool _light1GoRight;
};

class TestSpotLight : public TestBase
{
public:
	TestSpotLight();
	virtual void Step();
	Sprite3D* _sp;
	float _dirY;
	bool _addY;
	SpotLight* _spotLight;
};

class TestNormalMap : public TestBase
{
public:
	TestNormalMap();
	virtual void Step();
	Sprite3D* _sp;
	Texture2D* _normal_texture;
	Texture2D* _un_normal_texture;
};

class TestBillBoard : public TestBase
{
public:
	TestBillBoard();
	virtual void Step();
};

class TestShadowMap : public TestBase
{
public:
	TestShadowMap();
	virtual void Step();
	Mesh* _skelonMesh;
	float _lightX;
	bool _lightXAdd;
	DirectionLight* _dirlight;
};

struct TestData
{
	TestData(string _testName, std::function<TestBase*()> _testCreate)
	{
		testName = _testName;
		testCreate = _testCreate;
	}
	string testName;
	std::function<TestBase*()> testCreate;
};

class Tests
{
public:
	Tests()
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

	vector<TestData> _tests;
};


