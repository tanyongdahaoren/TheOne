#pragma once

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define INVALID_OGL_VALUE 0xffffffff

#include <string>
using namespace std;

//all shader name
static const string shader_position_color = "shader_position_color";
static const string shader_position_texure_2D = "shader_position_texure_2D";
static const string shader_position_texure_3D = "shader_position_texure_3D";
static const string shader_base_light_3D = "shader_base_light_3D";
static const string shader_base_light_3D_with_normal_map = "shader_base_light_3D_with_normal_map";

//attribute
enum eShaderVertAttribute
{
	eShaderVertAttribute_pos,
	eShaderVertAttribute_color,
	eShaderVertAttribute_texcood,
	eShaderVertAttribute_normal,
	eShaderVertAttribute_blend_weight,
	eShaderVertAttribute_blend_index,
	eShaderVertAttribute_tangent,

	eShaderVertAttribute_num,
};

static const char* ATTRIBUTE_NAME_POSITION = "a_model_pos";
static const char* ATTRIBUTE_NAME_COLOR = "a_color";
static const char* ATTRIBUTE_NAME_TEX_COORD = "a_tex_coord";
static const char* ATTRIBUTE_NAME_NORMAL = "a_model_normal";
static const char* ATTRIBUTE_NAME_BLEND_WEIGHT = "a_blend_weight";
static const char* ATTRIBUTE_NAME_BLEND_INDEX = "a_blend_index";

//uniform
static const string UNIFORM_MVP = "u_MVP";
static const string UNIFORM_M = "u_M";
static const string UNIFORM_V = "u_V";
static const string UNIFORM_TEXTURE_COLOR_SAMPLER = "u_texture_color_sampler";
static const string UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER = "u_texture_normal_map_sampler";