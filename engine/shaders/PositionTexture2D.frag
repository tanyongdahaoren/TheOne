const char* PositionTexture2D_frag = STRINGIFY(

in vec2 o_tex_coord;

out vec4 color;

void main()
{
	color = texture2D(u_sampler, o_tex_coord.xy);
}
);
