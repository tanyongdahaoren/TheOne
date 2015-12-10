#pragma once

#include "Sprite3D.h"

class TextureCubeMap;
class SkyBox : public Sprite3D
{
public:
	SkyBox();

	void InitWithTextureCubeMap(TextureCubeMap* textureCubeMap);
protected:
	 
};