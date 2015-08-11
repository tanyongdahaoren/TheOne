const char* PositionColor_frag = STRINGIFY(

in vec3 o_frag_color;

out vec3 color;

void main()
{
	color = o_frag_color;
}
);