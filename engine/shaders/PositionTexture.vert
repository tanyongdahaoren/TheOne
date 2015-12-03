const char* PositionTexture_vert = STRINGIFY(

in vec3 a_model_pos;
in vec2 a_tex_coord;
in vec3 a_model_normal;

out vec2 o_tex_coord;

void main()
{
	gl_Position = u_MVP * vec4(a_model_pos,1);
	o_tex_coord = a_tex_coord;
}
);