#include "Mesh.h"
#include "ShaderValue.h"

Mesh::Mesh()
	: _vao(INVALID_OGL_VALUE)
	, _vbo(INVALID_OGL_VALUE)
	, _ebo(INVALID_OGL_VALUE)
{

}

Mesh::~Mesh()
{
	if(_vbo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_vbo);
	if (_ebo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_ebo);
	if (_vao != INVALID_OGL_VALUE)
		glDeleteVertexArrays(1, &_vao);
}

void Mesh::GenBuffers()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers( 1, &_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, _vbo );
	glBufferData(GL_ARRAY_BUFFER, stridePerVertex * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	int offsetSizeBytes = 0;
	for (auto it:attribs)
	{
		MeshVertexAttrib& attrib = it.second;
		
		glEnableVertexAttribArray(attrib.vertexAttrib);
		glVertexAttribPointer(attrib.vertexAttrib, attrib.size, GL_FLOAT, GL_FALSE, stridePerVertex, (GLvoid *)offsetSizeBytes);
		glDisableVertexAttribArray(attrib.vertexAttrib);
		
		offsetSizeBytes += attrib.attribSizeBytes;
	}

	glGenBuffers( 1, &_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray(0);
}

void Mesh::UseBuffers()
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	for (auto it : attribs)
	{
		glEnableVertexAttribArray(it.first);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	for (auto it : attribs)
	{
		glDisableVertexAttribArray(it.first);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	int offset = 0;
	vec3 v0_pos, v1_pos, v2_pos;
	vec3 v1, v2;
	float* pv = nullptr;
	vec3 tempNormal;
	for (unsigned int i = 0; i < indices.size(); i+=3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		pv = &vertices[i * sizePerVertex];
		v0_pos = vec3(*pv, *(pv + 1), *(pv + 2));

		pv = &vertices[((i + 1) * sizePerVertex)];
		v1_pos = vec3(*pv, *(pv + 1), *(pv + 2));

		pv = &vertices[((i + 2) * sizePerVertex)];
		v2_pos = vec3(*pv, *(pv + 1), *(pv + 2));

		v1 = v1_pos - v0_pos;
		v2 = v2_pos - v0_pos;

		tempNormal = cross(v1, v2);

		int v0_normal_idx = GetVertexAttribIdx(index0, eShaderVertAttribute_normal);
		vec3 v0_normal = vec3(vertices[v0_normal_idx], vertices[v0_normal_idx + 1], vertices[v0_normal_idx + 2]);
		v0_normal += tempNormal;
		SetVertexAttribValue(index0, eShaderVertAttribute_normal, &v0_normal[0]);

		int v1_normal_idx = GetVertexAttribIdx(index1, eShaderVertAttribute_normal);
		vec3 v1_normal = vec3(vertices[v1_normal_idx], vertices[v1_normal_idx + 1], vertices[v1_normal_idx + 2]);
		v1_normal += tempNormal;
		SetVertexAttribValue(index1, eShaderVertAttribute_normal, &v1_normal[0]);

		int v2_normal_idx = GetVertexAttribIdx(index2, eShaderVertAttribute_normal);
		vec3 v2_normal = vec3(vertices[v2_normal_idx], vertices[v2_normal_idx + 1], vertices[v2_normal_idx + 2]);
		v2_normal += tempNormal;
		SetVertexAttribValue(index2, eShaderVertAttribute_normal, &v2_normal[0]);
	}

	int vNum = vertices.size() / sizePerVertex;
	int normalOffset = attribs[eShaderVertAttribute_normal].offset;
	float* pNormal = nullptr;
	for (int i = 0; i < vNum; i++)
 	{
		int normal_idx = GetVertexAttribIdx(i, eShaderVertAttribute_normal);
		vec3 normal = vec3(vertices[normal_idx], vertices[normal_idx + 1], vertices[normal_idx + 2]);
		normal = normalize(normal);
		SetVertexAttribValue(i, eShaderVertAttribute_normal, &normal[0]);
 	}
}

int Mesh::GetVertexAttribIdx(int idx, int attrib)
{
	return idx * sizePerVertex + attribs[attrib].offset;
}

void Mesh::SetVertexAttribValue(int idx, int attrib, float* pValue)
{
	int& attribSize = attribs[attrib].size;
	int attribIdx = GetVertexAttribIdx(idx, attrib);

	for (int i = 0; i < attribSize; i++)
	{
		vertices[attribIdx + i] = *(pValue + i);
	}
}

void Mesh::CalcTangents()
{

}
