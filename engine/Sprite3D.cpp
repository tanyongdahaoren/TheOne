#include "Sprite3D.h"
#include "ShaderValue.h"
#include "ShaderManager.h"

Sprite3D::Sprite3D()
	: _mesh(NULL)
	, _texture2D(NULL)
	, _program(NULL)
{
}

Sprite3D::~Sprite3D()
{
	SAFE_RELEASE(_texture2D);
	SAFE_RELEASE(_mesh);
}

bool Sprite3D::InitWithMesh(Mesh* mesh)
{
	SAFE_RELEASE(_mesh);
	_mesh = mesh;
	_mesh->Retain();

	SetShader(shader_position_texure_3D);

	return true;
}

void Sprite3D::SetShader(string shaderName)
{
	_program = ShaderManager::GetInstance()->GetShader(shaderName);
}

void Sprite3D::SetTexture2D(Texture2D* texture2D)
{
	SAFE_RELEASE(_texture2D);
	texture2D->Retain();
	_texture2D = texture2D;
}

void Sprite3D::Draw(Camera* camera)
{
	//glEnable(GL_CULL_FACE);

	_program->Active();

	_program->SetUniformLocationWith1i(UNIFORM_NAME_SAMPLER, 0);
	
	const mat4& projectTransform = camera->GetProjectTransform();
	const mat4& viewTransform = camera->GetViewTransform();
	glm::mat4 MVP = projectTransform * viewTransform * _toWorldTransform;
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_NAME_M, &_toWorldTransform[0][0]);
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_NAME_V, &viewTransform[0][0]);
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_NAME_MVP, &MVP[0][0]);

	_program->CustomEffect();

	_texture2D->Bind(GL_TEXTURE0);

	_mesh->UseBuffers();

	Node::Draw(camera);
}

void Sprite3D::Visit(const mat4& parentToWorldTransform)
{
	Node::Visit(parentToWorldTransform);
}
