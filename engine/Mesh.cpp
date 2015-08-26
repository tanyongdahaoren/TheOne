#include "Mesh.h"
#include "ShaderValue.h"

Mesh::Mesh()
	: _vao(INVALID_OGL_VALUE)
	, _ebo(INVALID_OGL_VALUE)
{

}

Mesh::~Mesh()
{
	if (_ebo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_ebo);
	if (_vao != INVALID_OGL_VALUE)
		glDeleteVertexArrays(1, &_vao);
	for (auto it:_vbos)
	{
		glDeleteVertexArrays(1, &it.second);
	}
}

void Mesh::GenBuffers()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	for (auto it:attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexDatas[attribType].size(), &vertexDatas[attribType][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribType);
		glVertexAttribPointer(attribType, attrib.size, GL_FLOAT, GL_FALSE, 0, 0);
		_vbos.insert(make_pair(attribType, vbo));
	}

	glGenBuffers( 1, &_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW );

	glBindVertexArray(0);
}

void Mesh::UseBuffers()
{
	glBindVertexArray(_vao);

 	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

bool Mesh::HaveAttribute(int attrib)
{
	auto it = attribs.find(attrib);
	if (it == attribs.end())
	{
		return false;
	}
	return true;
}

void Mesh::CalcNormals()
{
	vertexDatas[eShaderVertAttribute_normal].resize(vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		float* pPos0 = GetVertex(eShaderVertAttribute_pos, index0);
		float* pPos1 = GetVertex(eShaderVertAttribute_pos, index1);
		float* pPos2 = GetVertex(eShaderVertAttribute_pos, index2);
		vec3 pos0 = vec3(*pPos0, *(pPos0 + 1), *(pPos0 + 2));
		vec3 pos1 = vec3(*pPos1, *(pPos1 + 1), *(pPos1 + 2));
		vec3 pos2 = vec3(*pPos2, *(pPos2 + 1), *(pPos2 + 2));
		vec3 v1 = pos1 - pos0;
		vec3 v2 = pos2 - pos0;
		vec3 normal = normalize(cross(v1, v2));

		float* pNormal0 = GetVertex(eShaderVertAttribute_normal, index0);
		float* pNormal1 = GetVertex(eShaderVertAttribute_normal, index1);
		float* pNormal2 = GetVertex(eShaderVertAttribute_normal, index2);
		vec3 normal0 = vec3(*pNormal0, *(pNormal0 + 1), *(pNormal0 + 2));
		vec3 normal1 = vec3(*pNormal1, *(pNormal1 + 1), *(pNormal1 + 2));
		vec3 normal2 = vec3(*pNormal2, *(pNormal2 + 1), *(pNormal2 + 2));
		normal0 += normal;
		normal1 += normal;
		normal2 += normal;
		SetVertex(eShaderVertAttribute_normal, index0, &normal0[0]);
		SetVertex(eShaderVertAttribute_normal, index1, &normal1[0]);
		SetVertex(eShaderVertAttribute_normal, index2, &normal2[0]);
	}

	vector<float>& normals = vertexDatas[eShaderVertAttribute_normal];
	for (unsigned int i = 0; i < normals.size(); i += 3)
	{
		vec3 normal = normalize(vec3(normals[i], normals[i + 1], normals[i + 2]));
		normals[i] = normal.x;
		normals[i + 1] = normal.y;
		normals[i + 2] = normal.z;
	}
}

void Mesh::CalcTangents()
{
	vertexDatas[eShaderVertAttribute_tangent].resize(vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		//pos
		float* pPos0 = GetVertex(eShaderVertAttribute_pos, index0);
		float* pPos1 = GetVertex(eShaderVertAttribute_pos, index1);
		float* pPos2 = GetVertex(eShaderVertAttribute_pos, index2);
		vec3 pos0 = vec3(*pPos0, *(pPos0 + 1), *(pPos0 + 2));
		vec3 pos1 = vec3(*pPos1, *(pPos1 + 1), *(pPos1 + 2));
		vec3 pos2 = vec3(*pPos2, *(pPos2 + 1), *(pPos2 + 2));

		//uv
		float* pUV0 = GetVertex(eShaderVertAttribute_texcood, index0);
		float* pUV1 = GetVertex(eShaderVertAttribute_texcood, index1);
		float* pUV2 = GetVertex(eShaderVertAttribute_texcood, index2);
		vec2 uv0 = vec2(*pUV0, *(pUV0 + 1));
		vec2 uv1 = vec2(*pUV1, *(pUV1 + 1));
		vec2 uv2 = vec2(*pUV2, *(pUV2 + 1));
		
		// Edges of the triangle : postion delta
		vec3 deltaPos1 = pos1 - pos0;
		vec3 deltaPos2 = pos2 - pos0;

		// UV delta
		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;

		float* pTangent0 = GetVertex(eShaderVertAttribute_tangent, index0);
		float* pTangent1 = GetVertex(eShaderVertAttribute_tangent, index1);
		float* pTangent2 = GetVertex(eShaderVertAttribute_tangent, index2);
		vec3 tangent0 = vec3(*pTangent0, *(pTangent0 + 1), *(pTangent0 + 2));
		vec3 tangent1 = vec3(*pTangent1, *(pTangent1 + 1), *(pTangent1 + 2));
		vec3 tangent2 = vec3(*pTangent2, *(pTangent2 + 1), *(pTangent2 + 2));
		tangent0 += tangent;
		tangent1 += tangent;
		tangent2 += tangent;
		SetVertex(eShaderVertAttribute_tangent, index0, &tangent0[0]);
		SetVertex(eShaderVertAttribute_tangent, index1, &tangent1[0]);
		SetVertex(eShaderVertAttribute_tangent, index2, &tangent2[0]);
	}
}

float* Mesh::GetVertex(int attrib, int vertexIdx)
{
	int& size = attribs[attrib].size;
	return &vertexDatas[attrib][vertexIdx * size];
}

void Mesh::SetVertex(int attrib, int vertexIdx, float* pValue)
{
	float* p = GetVertex(attrib, vertexIdx);
	int& attribSize = attribs[attrib].size;
	for (int i = 0; i < attribSize; i++)
	{
		*(p + i) = *(pValue + i);
	}
}
