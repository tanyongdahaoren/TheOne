#pragma once

#include "Node.h"
#include "MeshManager.h"
#include "Texture2D.h"
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

	void SetTexture(Texture2D* texture2D);

	void SetNormalTexture(Texture2D* texture2D);
protected:
	virtual void Render(Camera* camera);

	virtual void RenderShadowMapping(const mat4& lightTransform);
protected:
	Mesh* _mesh;
	Texture2D* _texture;
	Texture2D* _normalTexture;
	Shader* _program;
	bool _cullBack;
#if DEBUG_SPRITE3D
	DrawPoints* _dp;
	DrawLines* _dl;
#endif

	Shader* _shadowShader;
};