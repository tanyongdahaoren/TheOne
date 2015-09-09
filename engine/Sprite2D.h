#pragma once

#include "Sprite3D.h"

class Sprite2D : public Sprite3D
{
public:
	Sprite2D();

	void InitWithTexture2D(Texture2D* texture2d, Rect uv = Rect(0,0,1.0f,1.0f));

	void SetAnchorPoint(vec2 v);
protected:
	void UpdateMeshVertexPos(Mesh* mesh);
	virtual void Draw(Camera* camera);
protected:
	bool _meshVertexDirty;
	vec2 _anchorPoint;
};