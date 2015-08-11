#include "Director.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include "GLHeader.h"
#include <GL/glew.h>
#ifdef GLEW_MX
GLEWContext _glewctx;
#endif

GLFWwindow* window;

#include <windows.h>

//engine
#include "Defines.h"
#include "AutoReleasePool.h"
#include "Tree.h"
#include "ShaderManager.h"
#include "FileUtils.h"
//test
#include "DrawNode.h"
#include "Sprite2D.h"
#include "Sprite3D.h"
#include "EasyImage.h"
#include "Texture2D.h"
#include "MeshManager.h"

static Tree* sCurrentTree = NULL;

Director* Director::GetInstance()
{
	static Director s_instance;

	return &s_instance;
}

Director::Director()
{
}

Director::~Director()
{
	AutoReleasePool::DestroyInstance();
	ShaderManager::DestroyInstance();
	FileUtils::DestroyInstance();
}

Sprite3D* sp3d = NULL;

int Director::Run()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(sWinW, sWinH, "The One", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0, 0.0f);
	
	//main loop
	double last, current, during = 0.0;
	last = glfwGetTime();

	//init eninge

	ShaderManager::GetInstance();
	FileUtils::GetInstance();
	
	//-----------------------------
	//test code
	//-----------------------------
	Tree* tree = new Tree;
	_trees.push_back(tree);

 	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 10000.0);
	//camera->orthographic(sWinW, sWinH, .1, 100);
	vec3 eye(-30, 10, 30);
	vec3 center(0, 0, 0);
	camera->LookAt(eye, center);
	tree->AddCamera(camera);

	Node* par = new Node;
	tree->AddChild(par);

	//dir light
	BaseLight::SetSpecularIntensity(1);
	BaseLight::SetSpecularPower(32);

	DirectionLight* dirlight = new DirectionLight;
	dirlight->SetColor(Color3F::WHITE);
	dirlight->SetDirection(vec3(1,0,0));
	dirlight->SetAmbientIntensity(0);
	dirlight->SetDiffuseIntensity(0);
	par->AddChild(dirlight);

	//2d sprite
	{
// 		EasyImage* image = new EasyImage;
// 		image->initWithFileName("2d.gif");
// 		Texture2D* texture = new Texture2D;
// 		texture->loadWithImage(image);
// 		texture->retain();
// 		Sprite2D* sp = new Sprite2D;
// 		sp->setPosition(vec3(50,50,0));
// 		sp->setScale(vec3(.3f, .3f, 1));
// 		sp->setRotation(vec3(0, 60, 0));
// 		sp->initWithTexture2D(texture);
// 		par->addChild(sp);
	}
	
	//3d sprite
// 	{
// 		auto meshs = MeshManager::GetInstance()->LoadMeshFromFile("phoenix_ugv.md2");
// 		EasyImage* image = new EasyImage;
// 		image->InitWithFileName("phoenix.pcx");
// 		Texture2D* texture = new Texture2D;
// 		texture->LoadWithImage(image);
// 
// 		for (ssize_t i = 0; i < meshs->size(); i++)
// 		{
// 			Sprite3D* sp = new Sprite3D;
// 			sp->InitWithMesh(meshs->at(i));
// 			sp->SetTexture2D(texture);
// 			par->addChild(sp);
// 		}
// 	}
	
	{
		Mesh* mesh = new Mesh;
		V3F_T2F_V3N v1(vec3(-10, -10, 5.773f), vec2(0, 0)); mesh->vertices.push_back(v1);
		V3F_T2F_V3N v2(vec3(0.0f, -10, -11.5475f), vec2(0.5f, 0.0f)); mesh->vertices.push_back(v2);
		V3F_T2F_V3N v3(vec3(10, -10, 5.773f), vec2(1.0f, 0.0f)); mesh->vertices.push_back(v3);
		V3F_T2F_V3N v4(vec3(0.0f, 10, 0.0f), vec2(0.5f, 1.0f)); mesh->vertices.push_back(v4);

		mesh->indices.push_back(0);
		mesh->indices.push_back(3);
		mesh->indices.push_back(1);
		mesh->indices.push_back(1);
		mesh->indices.push_back(3);
		mesh->indices.push_back(2);
		mesh->indices.push_back(2);
		mesh->indices.push_back(3);
		mesh->indices.push_back(0);
		mesh->indices.push_back(1);
		mesh->indices.push_back(2);
		mesh->indices.push_back(0);
		mesh->CalcNormals();
		mesh->GenBuffers();
		
		EasyImage* image = new EasyImage;
		bool b = image->InitWithFileName("test.png");
		if (b)
		{
			Texture2D* texture = new Texture2D;
			texture->LoadWithImage(image);

			Sprite3D* sp = new Sprite3D;
			sp->InitWithMesh(mesh);
			sp->SetTexture2D(texture);
			par->AddChild(sp);

			sp3d = sp;
		}
	}
 	
	//line
	{
		DrawLines* n = new DrawLines;
		par->AddChild(n);
		n->DrawLine(vec3(0, 0, 0), vec3(500, 0, 0), Color3B::RED, Color3B::RED);
		n->DrawLine(vec3(0, 0, 0), vec3(0, 500, 0), Color3B::GREEN, Color3B::GREEN);
		n->DrawLine(vec3(0, 0, 0), vec3(0, 0, 500), Color3B::BLUE, Color3B::BLUE);
	}
	
	
#define N 60
	do{
		current = glfwGetTime();
		during += current - last;
		last = current;

		while (during > 1.0 / (double)N)
		{
			during -= 1.0 / (double)N;

			MainLoop();

			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

static float rot = 0;

void Director::MainLoop()
{
	//auto release
	AutoReleasePool::GetInstance()->Clear();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//travel all nodes
	for ( const auto &tree: _trees )
	{
		sCurrentTree = tree;

		sp3d->SetRotation(vec3(0, rot, 0));
		rot+=1;

		tree->Travel();
	}
}

void Director::End()
{
}

Tree* Director::GetCurrentTree()
{
	return sCurrentTree;
}


