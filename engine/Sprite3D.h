#pragma once

#include "Node.h"
#include "MeshManager.h"
#include "Texture.h"
#include "Shader.h"

#define DEBUG_SPRITE3D 0
#include "DrawNode.h"

class Sprite3D : public Node
{
public:
	Sprite3D();
	~Sprite3D();
public:
	bool InitWithMesh(Mesh* mesh);

	void SetShader(string shaderName);
protected:
	virtual void Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform);

	virtual void RenderShadowMapping(const mat4& lightProjTransform, const mat4& lightViewTransform);
protected:
	Mesh* _mesh;

	Shader* _program;
	bool _cullBack;
#if DEBUG_SPRITE3D
	DrawPoints* _dp;
	DrawLines* _dl;
#endif

	Shader* _shadowShader;
};