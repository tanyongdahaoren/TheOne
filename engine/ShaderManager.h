#pragma once

#include <functional>  
#include <map>
using namespace std;
#include "GLHeader.h"

#include "Shader.h"

class ShaderManager
{
public:
	static ShaderManager* GetInstance();

	static void DestroyInstance();

	Shader* LoadShader(string shaderName);

	Shader* GetShader(string shaderName);
private:
	ShaderManager();

	~ShaderManager();

	void LoadDefaultShaders();
public:
	map<string, Shader*> _shaders;
};