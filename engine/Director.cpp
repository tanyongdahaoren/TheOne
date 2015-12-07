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
#include "Camera.h"
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
		delta = delta * 0.1f;
		vec3 rotation = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera()->GetRotation();
		Director::GetInstance()->GetCurrentTree()->GetCurrentCamera()->SetRotation(rotation + vec3(delta.y, delta.x, 0));
	}
	
	sPreCursorPos = vec2(x, y);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
		return;
	Camera* camera = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera();
	mat4 m = camera->GetToWorldTransform();
	vec3 forward = vec3(-m[2][0], -m[2][1], -m[2][2]);
	vec3 right = vec3(m[0][0], m[0][1], m[0][2]);
	vec3 pos = camera->GetPosition();

	static float sCameraMoveSpeed = 1.5;
	switch (key)
	{
	case GLFW_KEY_W:
		{
			pos += forward * sCameraMoveSpeed;
		}
		break;
	case GLFW_KEY_S:
		{
			pos -= forward * sCameraMoveSpeed;
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
	camera->SetPosition(pos);
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

	Node* par = new Node;
	tree->AddChild(par);
	
	//camera
	Camera* camera = new Camera;
	camera->Perspective(60, sWinW / sWinH, 0.1f, 1000.0);
	//camera->Orthographic(30, 30 * sWinH / sWinW, .1, 100);
	vec3 eye(10, 10, 10);
	vec3 center(0, 0, 0);
	camera->SetPosition(eye);
	camera->LookAt(center);
	//camera->SetMouseSpeed(0.5f);
	par->AddChild(camera);

	BaseLight::SetSpecularIntensity(1);
	BaseLight::SetSpecularPower(32);

	//line
	{
		DrawLines* n = new DrawLines;
		par->AddChild(n);
		n->DrawLine(vec3(0, 0, 0), vec3(500, 0, 0), Color3B::RED, Color3B::RED);
		n->DrawLine(vec3(0, 0, 0), vec3(0, 500, 0), Color3B::GREEN, Color3B::GREEN);
		n->DrawLine(vec3(0, 0, 0), vec3(0, 0, 500), Color3B::BLUE, Color3B::BLUE);
	}

	//dir light
	DirectionLight* dirlight = new DirectionLight;
	dirlight->SetColor(Color3F::WHITE);
	dirlight->SetDirection(vec3(-10, -10, -10));
	dirlight->SetAmbientIntensity(0.1);
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
	
	
	bool is_show_3dsp = true;
	bool is_show_2dsp = true;
	bool is_show_skelon = true;
	bool is_show_shadow = true;
	Sprite2D* sp2d = NULL;

	//3d sprite/
	if (is_show_3dsp)
 	{
		EasyImage* image = new EasyImage;
		image->InitWithFileName("bricks.jpg");
		Texture2D* texture = new Texture2D;
		texture->LoadWithImage(image);
		texture->SetWrapType(eWrapType_reapeat);

		EasyImage* normal_image = new EasyImage;
		normal_image->InitWithFileName("bricks_normal_map.jpg");
		Texture2D* normal_texture = new Texture2D;
		normal_texture->LoadWithImage(normal_image);
		normal_texture->SetWrapType(eWrapType_reapeat);

		Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("box.obj", false, true);
		mesh->GenBuffers();
		mesh->SetNormalTexture(normal_texture);
		mesh->SetTexture(texture);

		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(mesh);
		par->AddChild(sp);
		sp->SetPosition(vec3(4,3,0));

		sp3d = sp;
	}

	if (is_show_shadow)
	{
		dirlight->OpenShadow(true);

		EasyImage* image = new EasyImage;
		image->InitWithFileName("white.png");
		Texture2D* texture = new Texture2D;
		texture->LoadWithImage(image);
		texture->SetWrapType(eWrapType_reapeat);

		Mesh* mesh = MeshManager::GetInstance()->LoadMeshFromFile("room_thickwalls.obj", false, false);
		mesh->GenBuffers();
		mesh->SetTexture(texture);
		
		Sprite3D* sp = new Sprite3D;
		sp->InitWithMesh(mesh);
		par->AddChild(sp);
		sp->SetRotation(vec3(0,180,0));
	}

	Mesh* skelonMesh = NULL;
	//3d skelon sprite/
	if (is_show_skelon)
	{
		skelonMesh = MeshManager::GetInstance()->LoadMeshFromFile("boblampclean.md5mesh", true, false);
		skelonMesh->GenBuffers();
		skelonMesh->GenTextures();
	
		Sprite3D* sp = new Sprite3D;
		sp->SetPosition(vec3(2,0,2));
		sp->InitWithMesh(skelonMesh);
		par->AddChild(sp);
		sp->SetScale(vec3(0.05,0.05,0.05));
		sp->SetRotation(vec3(-90,0,0));
		sp3d = sp;
	}

	//2d sprite
	if (is_show_2dsp)
	{
		EasyImage* image = new EasyImage;
		image->InitWithFileName("number.png");
		Texture2D* texture = new Texture2D;
		texture->LoadWithImage(image);

		sp2d = new Sprite2D;
		sp2d->InitWithTexture2D(texture);
		sp2d->SetPosition(vec3(-3, 1, 2));
		sp2d->SetAnchorPoint(vec3(0.5, 0.5, 0));
		sp2d->SetScale2D(vec2(0.03, 0.03));
		sp2d->EnableBillBoard(Sprite2D::eBillBoardType_rotate_y);
		sp2d->SetShader(shader_position_texture);
		par->AddChild(sp2d);
	}
 	
	
	
	
#define N 60
	do{
		current = glfwGetTime();
		during += current - last;
		last = current;
		
		while (during > 1.0 / (double)N)
		{
			during -= 1.0 / (double)N;

			static float r = 0;
			r+=0.1f;
			
			if (skelonMesh)
			{
				skelonMesh->BoneTransform(r);
			}

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

void Director::MainLoop()
{
	//auto release
	AutoReleasePool::GetInstance()->Clear();
	
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
