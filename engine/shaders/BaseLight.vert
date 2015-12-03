const char* BaseLight_vert = STRINGIFY(

in vec3 a_model_pos;
in vec2 a_tex_coord;
in vec3 a_model_normal;
in vec3 a_model_tangent;

out vec3 o_world_pos;
out vec2 o_tex_coord;
out vec3 o_world_normal;
out vec3 o_world_tangent;

void main()
{
	if(u_open_skelon == 1)
	{
		int idx0 = int(a_blend_index[0]);
		int idx1 = int(a_blend_index[1]);
		int idx2 = int(a_blend_index[2]);
		int idx3 = int(a_blend_index[3]);
		mat4 boneTransform = u_bone[idx0] * a_blend_weight[0];
			boneTransform += u_bone[idx1] * a_blend_weight[1];
			boneTransform += u_bone[idx2] * a_blend_weight[2];
			boneTransform += u_bone[idx3] * a_blend_weight[3];

		vec4 posL      = boneTransform * vec4(a_model_pos,1.0);
		vec4 normalL   = boneTransform * vec4(a_model_normal, 0.0);
		
		gl_Position    = u_MVP * posL;
		o_world_normal = (u_M * normalL).xyz;
		o_world_pos    = (u_M * posL).xyz;
		o_tex_coord    = a_tex_coord;
		if(u_open_shadow == 1)
		{
			o_pos_light_camera = u_light_MVP * posL;
		}
	}
	else
	{
		gl_Position    = u_MVP * vec4(a_model_pos,1.0);
		o_world_normal = (u_M * vec4(a_model_normal, 0.0)).xyz;
		o_world_pos    = (u_M * vec4(a_model_pos, 1.0)).xyz;
		o_tex_coord    = a_tex_coord;
		if(u_open_normal_map == 1)
		{
			o_world_tangent = (u_M * vec4(a_model_tangent, 1.0)).xyz;
		}
		if(u_open_shadow == 1)
		{
			o_pos_light_camera = u_light_MVP * vec4(a_model_pos, 1.0);
		}
	}
}
);