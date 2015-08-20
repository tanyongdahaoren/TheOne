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
	bool InitWithModel(MeshManager* model, int idx);

	bool InitWithMesh(Mesh* mesh);

	void SetShader(string shaderName);

	void SetTexture2D(Texture2D* texture2D);

protected:
	virtual void Visit(const mat4& parentToWorldTransform);
	void InitProgram();
	virtual void Draw(Camera* camera);
private:
	Mesh* _mesh;
	Texture2D* _texture2D;
	Shader* _program;

#if DEBUG_SPRITE3D
	DrawPoints* _dp;
	DrawLines* _dl;
#endif
};