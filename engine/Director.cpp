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

static bool sDragMouse = false;
static vec2 sPreCursorPos = vec2(sWinW/2, sWinH/2);
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	//printf("Cursor position: %f %f\n", x, y);

	if (sDragMouse)
	{
		vec2 delta = sPreCursorPos - vec2(x, y);
		Director::GetInstance()->GetCurrentTree()->GetCurrentCamera()->ChangeCameraAngle(delta);
	}
	
	sPreCursorPos = vec2(x, y);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
		return;
	Camera* camera = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera();
	vec3 dir = camera->GetDirection();
	vec3 right = camera->GetRight();
	vec3 pos = camera->GetEyePos();

	static float sCameraMoveSpeed = 1.5;
	switch (key)
	{
	case GLFW_KEY_W:
		{
			pos += dir * sCameraMoveSpeed;
		}
		break;
	case GLFW_KEY_S:
		{
			pos -= dir * sCameraMoveSpeed;
		}
		break;
	case GLFW_KEY_A:
		{
			pos -= right * sCameraMoveSpeed;
		}
		break;
	case GLFW_KEY_D:
		{
			pos += right * sCameraMoveSpeed;
		}
		break;
	}
	camera->SetEyePos(pos);
}

static const char* get_button_name(int button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		return "left";
	case GLFW_MOUSE_BUTTON_RIGHT:
		return "right";
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return "middle";
	default:
	{
		static char name[16];
		sprintf(name, "%i", button);
		return name;
	}
	}
}

static const char* get_mods_name(int mods)
{
	static char name[512];

	if (mods == 0)
		return " no mods";

	name[0] = '\0';

	if (mods & GLFW_MOD_SHIFT)
		strcat(name, " shift");
	if (mods & GLFW_MOD_CONTROL)
		strcat(name, " control");
	if (mods & GLFW_MOD_ALT)
		strcat(name, " alt");
	if (mods & GLFW_MOD_SUPER)
		strcat(name, " super");

	return name;
}

static const char* get_action_name(int action)
{
	switch (action)
	{
	case GLFW_PRESS:
		return "pressed";
	case GLFW_RELEASE:
		return "released";
	case GLFW_REPEAT:
		return "repeated";
	}

	return "caused unknown action";
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		sDragMouse = true;
	}
	else
	{
		sDragMouse = false;
	}
	printf("Mouse button %i (%s) (with%s) was %s\n",
		button,
		get_button_name(button),
		get_mods_name(mods),
		get_action_name(action));
}

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

	glfwSetCursorPos(window, sWinW/2, sWinH/2);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background
	glClearColor(0.0f, 0, .3, 0.0f);
	
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
	camera->Perspective(60, sWinW / sWinH, 0.1f, 1000.0);
	//camera->orthographic(sWinW, sWinH, .1, 100);
	vec3 eye(30, 30, 30);
	vec3 center(0, 0, 0);
	camera->SetEyePos(eye);
	camera->SetTarget(center);
	camera->SetMouseSpeed(0.5f);
	tree->AddCamera(camera);

	Node* par = new Node;
	tree->AddChild(par);
	
	BaseLight::SetSpecularIntensity(1.5);
	BaseLight::SetSpecularPower(32);

	{
		//dir light
		DirectionLight* dirlight = new DirectionLight;
		dirlight->SetColor(Color3F::WHITE);
		dirlight->SetDirection(vec3(-1, -1, -1));
		dirlight->SetAmbientIntensity(0.2);
		dirlight->SetDiffuseIntensity(0.8);
		par->AddChild(dirlight);

// 		PointLight* pointLight1 = new PointLight;
// 		pointLight1->SetColor(Color3F::WHITE);
// 		pointLight1->SetAmbientIntensity(0);
// 		pointLight1->SetDiffuseIntensity(.1);
// 		pointLight1->SetConstant(1.0f);
// 		pointLight1->SetLinear(0.1f);
// 		pointLight1->SetExp(0);
// 		pointLight1->SetPosition(vec3(-5,1,-1));
// 		par->AddChild(pointLight1);

// 		SpotLight* spotLight = new SpotLight;
// 		spotLight->SetColor(Color3F::WHITE);
// 		spotLight->SetAmbientIntensity(0);
// 		spotLight->SetDiffuseIntensity(0.3);
// 		spotLight->SetConstant(1.0f);
// 		spotLight->SetLinear(0.1f);
// 		spotLight->SetExp(0);
// 		spotLight->SetCutoff(0.8);
// 		spotLight->SetDirection(vec3(1, 0, 0));
// 		spotLight->SetPosition(vec3(-20,0,0));
// 		par->AddChild(spotLight);
	}
	
	//3d sprite
 	{
		auto meshs = MeshManager::GetInstance()->LoadMeshFromFile("box.obj", 
			aiProcess_Triangulate
			| aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs
			| aiProcess_CalcTangentSpace
			);
		Mesh* mesh = meshs->at(0);
		mesh->GenBuffers();
		
		EasyImage* image = new EasyImage;
		image->InitWithFileName("bricks.jpg");
		Texture2D* texture = new Texture2D;
		texture->LoadWithImage(image);

		EasyImage* normal_image = new EasyImage;
		normal_image->InitWithFileName("bricks_normal_map.jpg");
		Texture2D* normal_texture = new Texture2D;
		normal_texture->LoadWithImage(normal_image);

		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(mesh);
		sp->SetTexture(texture);
		sp->SetNormalTexture(normal_texture);
		par->AddChild(sp);

		sp3d = sp;
		
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
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glFrontFace(GL_CCW);

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//travel all nodes
	for ( const auto &tree: _trees )
	{
		sCurrentTree = tree;

		sp3d->SetRotation(vec3(0, rot, 0));
		rot+=0.1;

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
