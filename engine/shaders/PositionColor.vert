const char* PositionColor_vert = STRINGIFY(

in vec3 a_model_pos;
in vec3 a_color;

out vec3 o_frag_color;

void main()
{
	gl_Position =  u_MVP * vec4(a_model_pos,1);
	o_frag_color = a_color;
}
);