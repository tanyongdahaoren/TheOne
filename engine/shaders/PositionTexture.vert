const char* PositionTexture_vert = STRINGIFY(

in vec3 a_model_pos;
in vec2 a_tex_coord;

out vec2 o_tex_coord;

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

		vec4 posL   = boneTransform * vec4(a_model_pos,1.0);
		gl_Position = u_MVP * posL;
	}
	else
	{
		gl_Position = u_MVP * vec4(a_model_pos,1.0);
		
	}
	
	o_tex_coord = a_tex_coord;
}
);