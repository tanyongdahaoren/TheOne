#include "Sprite3D.h"
#include "ShaderValue.h"
#include "ShaderManager.h"
#include "Camera.h"

Sprite3D::Sprite3D()
	: _mesh(NULL)
	, _texture(NULL)
	, _normalTexture(NULL)
	, _program(NULL)
	, _cullBack(true)
	, _shadowShader(NULL)
{
	_shadowShader = ShaderManager::GetInstance()->GetShader(shader_depth_rtt);
}

Sprite3D::~Sprite3D()
{
	SAFE_RELEASE(_texture);
	SAFE_RELEASE(_normalTexture);
	SAFE_RELEASE(_mesh);
}

bool Sprite3D::InitWithMesh(Mesh* mesh)
{
	SAFE_RELEASE(_mesh);
	_mesh = mesh;
	_mesh->Retain();

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

void Sprite3D::SetTexture(Texture2D* texture2D)
{
	SAFE_RELEASE(_texture);
	texture2D->Retain();
	_texture = texture2D;
}

void Sprite3D::SetNormalTexture(Texture2D* texture2D)
{
	SAFE_RELEASE(_normalTexture);
	texture2D->Retain();
	_normalTexture = texture2D;
}

void Sprite3D::Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform)
{
	_program->Active();

	if (_cullBack)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	unsigned int textureFlag = 0;
	textureFlag |= COLOR_TEXTURE_INDEX;

	if (_mesh->HaveAttribute(eShaderVertAttribute_tangent) && _normalTexture)
	{
		textureFlag |= NORMAL_TEXTURE_INDEX;
		
		_normalTexture->Bind(NORMAL_TEXTURE);
		_program->SetUniformLocationWith1i(UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER, NORMAL_TEXTURE_INDEX);
	}

	_program->SetUniformLocationWith1i(UNIFORM_TEXTURE_COLOR_SAMPLER, COLOR_TEXTURE_INDEX);
	if (_texture)
	{
		_texture->Bind(COLOR_TEXTURE);
	}
	_program->Use(textureFlag, _mesh, _toWorldTransform, cameraViewTransform, cameraProjTransform);

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

void Sprite3D::RenderShadowMapping(const mat4& lightProjTransform, const mat4& lightViewTransform)
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

	_shadowShader->Use(0, _mesh, _toWorldTransform, lightViewTransform, lightProjTransform);

	_mesh->UseBuffers();
}
