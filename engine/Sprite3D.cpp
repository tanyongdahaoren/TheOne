#include "Sprite3D.h"
#include "ShaderValue.h"
#include "ShaderManager.h"

Sprite3D::Sprite3D()
	: _mesh(NULL)
	, _texture(NULL)
	, _normalTexture(NULL)
	, _program(NULL)
{
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

	bool haveTangent = _mesh->HaveAttribute(eShaderVertAttribute_tangent);
	if (haveTangent)
	{
		SetShader(shader_base_light_3D_with_normal_map);
	}
	else
	{
		SetShader(shader_base_light_3D);
	}	

#if DEBUG_SPRITE3D
	_dp = new DrawPoints;
	this->AddChild(_dp);
	_dl = new DrawLines;
	this->AddChild(_dl);
	int vNum = mesh->vertices.size() / mesh->sizePerVertex;
	for (int i = 0; i < vNum; i++)
	{
		int pos_idx = mesh->GetVertexAttribIdx(i, eShaderVertAttribute_pos);
		vec3 pos = vec3(mesh->vertices[pos_idx], mesh->vertices[pos_idx+1], mesh->vertices[pos_idx+2]);

		int normal_idx = mesh->GetVertexAttribIdx(i, eShaderVertAttribute_normal);
		vec3 normal = vec3(mesh->vertices[normal_idx], mesh->vertices[normal_idx + 1], mesh->vertices[normal_idx + 2]);

		_dp->DrawPoint(pos, Color3B::RED);
		_dl->DrawLine(pos, pos + normal, Color3B::RED, Color3B::GREEN);
	}
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

void Sprite3D::Draw(Camera* camera)
{
	_program->Active();

	const mat4& projectTransform = camera->GetProjectTransform();
	const mat4& viewTransform = camera->GetViewTransform();
	glm::mat4 MVP = projectTransform * viewTransform * _toWorldTransform;
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_M, &_toWorldTransform[0][0]);
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_V, &viewTransform[0][0]);
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_MVP, &MVP[0][0]);

	_program->CustomEffect();

	_program->SetUniformLocationWith1i(UNIFORM_TEXTURE_COLOR_SAMPLER, COLOR_TEXTURE_INDEX);
	_texture->Bind(COLOR_TEXTURE);
	if (_normalTexture)
	{
		_program->SetUniformLocationWith1i(UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER, NORMAL_TEXTURE_INDEX);
		_normalTexture->Bind(NORMAL_TEXTURE);
	}

	_mesh->UseBuffers();

	Node::Draw(camera);
}

void Sprite3D::Visit(const mat4& parentToWorldTransform)
{
	Node::Visit(parentToWorldTransform);
}
