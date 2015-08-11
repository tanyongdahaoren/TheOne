const char* PositionTexture2D_vert = STRINGIFY(

in vec2 a_model_pos;
in vec2 a_tex_coord;

out vec2 o_tex_coord;

void main()
{
	gl_Position = u_MVP * vec4(vec3(a_model_pos,0),1);
	o_tex_coord = a_tex_coord;
}
);