#pragma once

#include "Sprite3D.h"

class Sprite2D : public Sprite3D
{
public:
	enum eBillBoardType
	{
		eBillBoardType_nun,
		eBillBoardType_plane,
		eBillBoardType_rotate_y,
	};
public:
	Sprite2D();

	void InitWithTexture2D(Texture2D* texture2d, Rect uv = Rect(0,0,1.0f,1.0f));

	void EnableBillBoard(eBillBoardType type = eBillBoardType_plane);

	//-------------
	//For Sprite2D, make mesh's vertex not change because change texture by scale the transform
	//-------------
	virtual void SetScaleX(float x);
	virtual void SetScaleY(float y);
	void SetScale2D(vec2 scale);
	vec2 GetScale2D();
protected:
	//------------------
	//Not used in Sprite2D
	//------------------
	virtual void SetScale(vec3 scale);
	virtual vec3 GetScale();

	virtual void UpdateWorldTransorm(const mat4& parentToWorldTransform);
protected:
	vec2 _modelScale;

	eBillBoardType _billBoardType;
};