const char* PositionColor_frag = STRINGIFY(

in vec3 o_frag_color;

out vec4 color;

void main()
{
	color = vec4(o_frag_color, 1);
}
);