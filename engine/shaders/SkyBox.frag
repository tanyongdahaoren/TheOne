const char* SkyBox_frag = STRINGIFY(

in vec3 o_tex_coord;
uniform samplerCube u_texture_sky_box_sampler;

void main()
{
	gl_FragColor = texture(u_texture_sky_box_sampler, o_tex_coord);
}

);