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
#include "Texture.h"
#include "SkyBox.h"
#include "MeshManager.h"
#include "Tests.h"

static bool sDragMouse = false;
static vec2 sPreCursorPos = vec2(sWinW/2, sWinH/2);
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	//printf("Cursor position: %f %f\n", x, y);

	if (sDragMouse)
	{
		vec2 delta = sPreCursorPos - vec2(x, y);
		delta = delta * 0.1f;
		vec3 rotation = Director::GetInstance()->_trees[0]->GetCurrentCamera()->GetRotation();
		Director::GetInstance()->_trees[0]->GetCurrentCamera()->SetRotation(rotation + vec3(delta.y, delta.x, 0));
	}
	
	sPreCursorPos = vec2(x, y);
}
Vector<Texture2D*> _txs;
Tests TheOneTest;
int test_id = 0;
void reset_test();
extern float sCameraMoveSpeed;
Sprite2D* sp_test_name = NULL;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT && action != GLFW_PRESS)
		return;
	if (key == GLFW_KEY_LEFT)
	{
		test_id--;
		reset_test();
		return;
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		test_id++;
		reset_test();
		return;
	}
	
	Camera* camera = Director::GetInstance()->_trees[0]->GetCurrentCamera();
	if (!camera)
	{
		return;
	}

	mat4 m = camera->GetToWorldTransform();
	vec3 forward = vec3(-m[2][0], -m[2][1], -m[2][2]);
	vec3 right = vec3(m[0][0], m[0][1], m[0][2]);
	vec3 pos = camera->GetPosition();

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
	glClearColor(0.0f, 0, 0, 0.0f);
	
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

	BaseLight::SetSpecularIntensity(1);
	BaseLight::SetSpecularPower(32);

	{
		Tree* tree2 = new Tree;
		_trees.push_back(tree2);

		Camera* camera = new Camera;
		camera->Orthographic(sWinW, sWinH, -1, 1);
		vec3 eye(0, 0, 0.5);
		vec3 center(0, 0, 0);
		camera->SetPosition(eye);
		camera->LookAt(center);
		tree2->AddChild(camera);

		char buff[32];
		for (int i = 0; i < TheOneTest._tests.size(); i++)
		{
			sprintf(buff, "testnames/%d.png", i);
			Texture2D* t = new Texture2D;
			_txs.push_back(t);
			t->LoadTextureFromImage(buff);
		}

		sp_test_name = new Sprite2D;
		sp_test_name->InitWithTexture2D(_txs[0]);
		sp_test_name->SetShader(shader_position_texture);
		sp_test_name->SetPositionY(-sWinH/2 + 80);
		sp_test_name->SetPositionZ(0.1f);
		tree2->AddChild(sp_test_name);
	}

	reset_test();
		
#define N 60
	do{
		current = glfwGetTime();
		during += current - last;
		last = current;
		
		while (during > 1.0 / (double)N)
		{
			during -= 1.0 / (double)N;

			Node* curTest = Director::GetInstance()->_trees[0]->GetChildren().front();
			TestBase* c = dynamic_cast<TestBase*>(curTest);
			c->Step();

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
	for (int i = 0; i < _trees.size(); i++)
	{
		sCurrentTree = _trees[i];

		sCurrentTree->Travel(i);
	}
}

void Director::End()
{
}

Tree* Director::GetCurrentTree()
{
	return sCurrentTree;
}

void reset_test()
{
	if (test_id < 0)
	{
		test_id = TheOneTest._tests.size() - 1;
	}
	if (test_id > TheOneTest._tests.size() - 1)
	{
		test_id = 0;
	}

	sp_test_name->SetTexture(_txs[test_id]);

	Director::GetInstance()->_trees[0]->RemoveAllChildren();
	TestBase* par = TheOneTest._tests[test_id].testCreate();
	Director::GetInstance()->_trees[0]->AddChild(par);
}