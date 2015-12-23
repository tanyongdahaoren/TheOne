#include "SkyBox.h"

SkyBox::SkyBox()
	:_textureCubeMap(NULL)
{

}

void SkyBox::InitWithTextureCubeMap(TextureCubeMap* textureCubeMap)
{
	_textureCubeMap = textureCubeMap;

	Mesh* mesh = new Mesh;
	mesh->Retain();

	mesh->_entries.resize(1);
	mesh->_entries[0].MaterialIndex = 0;
	mesh->_entries[0].NumIndices = 36;
	mesh->_entries[0].BaseVertex = 0;
	mesh->_entries[0].BaseIndex = 0;

	mesh->_attribFlag = MeshAttribStep_pos;
	mesh->FillVertexAttributeWithFlag();

	vec3 vexBuf[] =
	{
		vec3(1, -1, 1), vec3(1, 1, 1), vec3(-1, 1, 1), vec3(-1, -1, 1),
		vec3(1, -1, -1), vec3(1, 1, -1), vec3(-1, 1, -1), vec3(-1, -1, -1)
	};
	for (int i = 0; i < ARRAY_SIZE(vexBuf); i++)
	{
		mesh->_vertexDatas[eShaderVertAttribute_pos].push_back(vexBuf[i].x);
		mesh->_vertexDatas[eShaderVertAttribute_pos].push_back(vexBuf[i].y);
		mesh->_vertexDatas[eShaderVertAttribute_pos].push_back(vexBuf[i].z);
	}

	const unsigned char idxBuf[] = { 2, 1, 0, 3, 2, 0, // font
		1, 5, 4, 1, 4, 0, // right
		4, 5, 6, 4, 6, 7, // back
		7, 6, 2, 7, 2, 3, // left
		2, 6, 5, 2, 5, 1, // up
		3, 0, 4, 3, 4, 7  // down
	};
	for (int i = 0; i < ARRAY_SIZE(idxBuf); i++)
	{
		mesh->_indices.push_back(idxBuf[i]);
	}

	mesh->GenBuffers();

	InitWithMesh(mesh);

	SetShader(shader_sky_box);
}

void SkyBox::Render(Camera* camera)
{
	GLint OldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
	GLint OldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	_program->Active();

	_textureCubeMap->Bind(CUBE_TEXTURE);

	unsigned int textureFlag = 0;

	_program->Use(textureFlag, _mesh, _toWorldTransform, camera);

	_mesh->UseBuffers();

	glCullFace(OldCullFaceMode);
	glDepthFunc(OldDepthFuncMode);
}
