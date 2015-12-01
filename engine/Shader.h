#pragma once

#include "GLHeader.h"
#include "ShaderValue.h"
#include <string>
#include <map>
using namespace std;
#include "MathH.h"

class Shader
{
public:
	Shader();
	~Shader();
	
	void InitWithProgramId(GLuint programID);

	void Active();

	void SetUniformLocationWith1i(string uniform, GLint i1);

	void SetUniformLocationWithMatrix4(string uniform, const mat4& matrix);

	virtual void CustomEffect(mat4 toWorldTransform){}

protected:
	virtual void InitUniformsLocation();
	GLint GetUniformLocation(const char* pUniformName);
protected:
	GLuint _programID;

	map<string, GLuint> _uniformsLocation;
};