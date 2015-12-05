#pragma once

#include "GLHeader.h"
#include "ShaderValue.h"
#include "ShaderModule.h"
#include <string>
#include <map>
using namespace std;
#include "MathH.h"

GLint extern GLGetUniformLocation(GLuint programID, const char* pUniformName);

class Mesh;
class Shader
{
public:
	Shader();
	~Shader();
	
	void Init(GLuint programID, Vector<ShaderModule*> modules);

	void Active();

	void SetUniformLocationWith1i(string uniform, GLint i1);

	void SetUniformLocationWithMatrix4(string uniform, const mat4& matrix);

	virtual void Use(unsigned int textureFlag, Mesh* mesh, mat4 toWorldTransform, mat4 viewTransform, mat4 projTransform);
	
protected:
	virtual void InitUniformsLocation();
		
protected:
	GLuint _programID;

	map<string, GLuint> _uniformsLocation;

	
	Vector<ShaderModule*> _mudules;
};

//shadow map
class ShaderShadowMap : public Shader
{
public:
	static string GetVertShader();
	static string GetFragShader();
protected:
	virtual void InitUniformsLocation();
	virtual void Use(Mesh* mesh, mat4 toWorldTransform);
protected:
	//for skelon
	GLuint _openSkelonLocation;
	GLuint _boneLocation[kMaxBoneNum];
};