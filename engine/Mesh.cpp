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
	glBufferData( GL_ARRAY_BUFFER, sizeof(V3F_T2F_V3N) * vertices.size(), &vertices[0], GL_STATIC_DRAW );

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glEnableVertexAttribArray(eShaderVertAttribute_texcood);
	glEnableVertexAttribArray(eShaderVertAttribute_normal);
	
	glVertexAttribPointer( eShaderVertAttribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F_V3N), (GLvoid *)offsetof(V3F_T2F_V3N, vertex) );
	glVertexAttribPointer(eShaderVertAttribute_texcood, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F_V3N), (GLvoid *)offsetof(V3F_T2F_V3N, texcood));
	glVertexAttribPointer(eShaderVertAttribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F_V3N), (GLvoid *)offsetof(V3F_T2F_V3N, normal));

	glGenBuffers( 1, &_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW );

 	glDisableVertexAttribArray(eShaderVertAttribute_pos);
 	glDisableVertexAttribArray(eShaderVertAttribute_texcood);
	glDisableVertexAttribArray(eShaderVertAttribute_normal);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray(0);
}

void Mesh::UseBuffers()
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glEnableVertexAttribArray(eShaderVertAttribute_texcood);
	glEnableVertexAttribArray(eShaderVertAttribute_normal);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(eShaderVertAttribute_pos);
	glDisableVertexAttribArray(eShaderVertAttribute_texcood);
	glEnableVertexAttribArray(eShaderVertAttribute_normal);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::CalcNormals()
{
/*	return;*/
	for (unsigned int i = 0; i < indices.size(); i+=3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		vec3 v1 = vertices[index1].vertex - vertices[index0].vertex;
		vec3 v2 = vertices[index2].vertex - vertices[index0].vertex;
		vec3 normal = cross(v1, v2);
		normal = normalize(normal);

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		vertices[i].normal = normalize(vertices[i].normal);
	}
}
