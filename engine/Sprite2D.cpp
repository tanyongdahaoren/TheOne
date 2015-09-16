#include "Sprite2D.h"
#include "ShaderManager.h"
#include "ShaderValue.h"
#include "Camera.h"

Sprite2D::Sprite2D()
	: _modelScale(1, 1)
	, _billBoardType(eBillBoardType_nun)
{

}

void Sprite2D::InitWithTexture2D(Texture2D* texture2d, Rect uv /*= Rect(0,0,1.0f,1.0f)*/)
{
	SetTexture(texture2d);

	_cullBack = false;

	Mesh* mesh = new Mesh;

	mesh->attribFlag = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs;

	mesh->FillVertexAttributeWithFlag();

	//------------
	//   v3------v2
	//    |      |
	//   v0------v1
	//---------------

	mesh->indices.push_back(0);
	mesh->indices.push_back(1);
	mesh->indices.push_back(3);

	mesh->indices.push_back(1);
	mesh->indices.push_back(2);
	mesh->indices.push_back(3);

	vec2 uvs[4] = { uv.origin, vec2(uv.getMaxX(), uv.getMinY()), vec2(uv.getMaxX(), uv.getMaxY()), vec2(uv.getMinX(), uv.getMaxY()) };
	for (int i = 0; i < 4; i++)
	{
		mesh->vertexDatas[eShaderVertAttribute_texcood].push_back(uvs[i].x);
		mesh->vertexDatas[eShaderVertAttribute_texcood].push_back(uvs[i].y);
	}

	vec2 pos[4] = { vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f) };
	for (int i = 0; i < 4; i++)
	{
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(pos[i].x);
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(pos[i].y);
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(0);
	}

	mesh->CalcNormals();

	mesh->GenBuffers();

	InitWithMesh(mesh);

	SetModelSize(Size3D(1, 1, 0));
	SetScaleX(1.0f);
	SetScaleY(1.0f);
}

void Sprite2D::SetScaleX(float x)
{
	_modelScale.x = x;
	Node::SetScaleX(x * _texture->_width);
}

void Sprite2D::SetScaleY(float y)
{
	_modelScale.y = y;
	Node::SetScaleY(y * _texture->_height);
}

void Sprite2D::SetScale2D(vec2 scale)
{
	SetScaleX(scale.x);
	SetScaleY(scale.y);
}

glm::vec2 Sprite2D::GetScale2D()
{
	return _modelScale;
}

void Sprite2D::SetScale(vec3 scale)
{
	Node::SetScale(scale);
}

glm::vec3 Sprite2D::GetScale()
{
	return Node::GetScale();
}

void Sprite2D::EnableBillBoard(eBillBoardType type /*= eBillBoardType_plane*/)
{
	_billBoardType = type;
}

void Sprite2D::CalculateWorldTransorm(const mat4& parentToWorldTransform)
{

}
