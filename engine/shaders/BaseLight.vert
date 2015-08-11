const char* BaseLight_vert = STRINGIFY(

in vec3 a_model_pos;
in vec2 a_tex_coord;
in vec3 a_model_normal;

out vec2 o_tex_coord;
out vec3 o_world_normal;
out vec3 o_world_pos;

void main()
{
	gl_Position = u_MVP * vec4(a_model_pos,1.0);
	o_world_normal = (u_M * vec4(a_model_normal, 0.0)).xyz;
	o_world_pos = (u_M * vec4(a_model_pos, 1.0)).xyz;
	o_tex_coord = a_tex_coord;
}
);