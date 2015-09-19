#include "Sprite2D.h"
#include "ShaderManager.h"
#include "ShaderValue.h"
#include "Camera.h"
#include "Director.h"
#include "Tree.h"

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

void Sprite2D::UpdateWorldTransorm(const mat4& parentToWorldTransform)
{
	Node::UpdateWorldTransorm(parentToWorldTransform);

	if (_billBoardType == eBillBoardType_nun)
	{
		return;
	}

	Camera* camera = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera();
	if (!camera)
	{
		return;
	}

	mat4 cameraWorldTransform = camera->_toWorldTransform;
	vec4 camX = cameraWorldTransform[0];
	vec4 camY = cameraWorldTransform[1];
	vec4 camZ = cameraWorldTransform[2];

	vec4 newX, newY, newZ;
	if (_billBoardType == eBillBoardType_plane)
	{	
		newX = normalize(camX);	
		newY = normalize(camY);
		newZ = normalize(camZ);
	}
	else if (_billBoardType == eBillBoardType_rotate_y)
	{
		newY = vec4(0, 1, 0, 0);
		vec3 z = cross(vec3(camX), vec3(newY)); z = normalize(z);
		newZ = vec4(z.x, z.y, z.z, 0);
		vec3 x = cross(vec3(newZ), vec3(newY)); x = normalize(x);
		newX = vec4(x.x, x.y, x.z, 0);
	}

	mat4 m = _toWorldTransform;

	float xlen = sqrtf(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]);
	float ylen = sqrtf(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]);
	float zlen = sqrtf(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2]);

	m[0] = newX * xlen;
	m[1] = newY * ylen;
	m[2] = newZ * zlen;

	_toWorldTransform = m;
}
