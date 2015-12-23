#include "Sprite3D.h"
#include "ShaderValue.h"
#include "ShaderManager.h"
#include "Camera.h"

Sprite3D::Sprite3D()
	: _mesh(NULL)
	, _program(NULL)
	, _cullBack(true)
	, _shadowShader(NULL)
{
	_shadowShader = ShaderManager::GetInstance()->GetShader(shader_depth_rtt);
}

Sprite3D::~Sprite3D()
{
}

bool Sprite3D::InitWithMesh(Mesh* mesh)
{
	_mesh = mesh;

	SetShader(shader_base_light);

#if DEBUG_SPRITE3D
	_dp = new DrawPoints;
	this->AddChild(_dp);
	_dl = new DrawLines;
	this->AddChild(_dl);
#endif

	return true;
}

void Sprite3D::SetShader(string shaderName)
{
	_program = ShaderManager::GetInstance()->GetShader(shaderName);
}

void Sprite3D::Render(Camera* camera)
{
	if (_cullBack)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	
	_program->Active();

	unsigned int textureFlag = 0;
	textureFlag |= COLOR_TEXTURE_INDEX;
	if (_mesh->HaveNormalMap())
	{
		textureFlag |= NORMAL_TEXTURE_INDEX;
	}

	_program->Use(textureFlag, _mesh, _toWorldTransform, camera);

	_mesh->UseBuffers();

#if DEBUG_SPRITE3D
	_dp->Clear();
	_dl->Clear();
	vector<float>& allPos = _mesh->_vertexDatas[eShaderVertAttribute_pos];
	vector<float>& allNormal = _mesh->_vertexDatas[eShaderVertAttribute_normal];
	for (int i = 0; i < allPos.size(); i += 3)
	{
		vec3 normal = vec3(allNormal[i], allNormal[i + 1], allNormal[i + 2]);
		vec3 pos = vec3(allPos[i], allPos[i + 1], allPos[i + 2]);

		_dp->DrawPoint(pos, Color3B::RED);
		_dl->DrawLine(pos, pos + normal, Color3B::RED, Color3B::GREEN);
	}
#endif
}

void Sprite3D::RenderShadowMapping(Camera* camera)
{
	if (_cullBack)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	_shadowShader->Active();

	_shadowShader->Use(0, _mesh, _toWorldTransform, camera);

	_mesh->UseBuffers();
}
