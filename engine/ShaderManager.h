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
		
	Shader* LoadShaders(const GLchar * vertShader, const GLchar * fragShader, string shaderName, std::function<Shader*()> newProgramFunc);

	Shader* GetShader(string shaderName);
private:
	ShaderManager();

	~ShaderManager();

	void LoadDefaultShaders();
public:
	map<string, Shader*> _shaders;
};