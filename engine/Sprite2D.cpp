#include "Sprite2D.h"
#include "ShaderManager.h"
#include "ShaderValue.h"
#include "Camera.h"

Sprite2D::Sprite2D()
	: _anchorPoint(0,0)
	, _meshVertexDirty(true)
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

	for (int i = 0; i < 4; i++)
	{
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(0);
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(0);
		mesh->vertexDatas[eShaderVertAttribute_pos].push_back(0);
	}

	UpdateMeshVertexPos(mesh);

	mesh->CalcNormals();

	mesh->GenBuffers();

	InitWithMesh(mesh);
}

void Sprite2D::SetAnchorPoint(vec2 v)
{
	_anchorPoint = v;

	_meshVertexDirty = true;
	_mesh->_bufferDirty = true;
}

void Sprite2D::UpdateMeshVertexPos(Mesh* mesh)
{
	float offsetX = _anchorPoint.x * _texture->_width;
	float offsetY = _anchorPoint.y * _texture->_height;

	vec2 pos[4] = { vec2(0, 0), vec2(_texture->_width, 0), vec2(_texture->_width, _texture->_height), vec2(0, _texture->_height) };
	for (int i = 0; i < 4; i++)
	{
		mesh->vertexDatas[eShaderVertAttribute_pos][i * 3] = pos[i].x - offsetX;
		mesh->vertexDatas[eShaderVertAttribute_pos][i * 3 + 1] = pos[i].y - offsetY;
	}

	_meshVertexDirty = false;
}

void Sprite2D::Draw(Camera* camera)
{
	if (_meshVertexDirty)
	{
		UpdateMeshVertexPos(_mesh);
	}

	Sprite3D::Draw(camera);
}
