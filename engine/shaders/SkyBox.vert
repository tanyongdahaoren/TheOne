const char* SkyBox_vert = STRINGIFY(

uniform mat4  u_camera_rot;

in vec3 a_model_pos;
out vec3 o_tex_coord;

void main()
{
	vec4 reflect = u_camera_rot * vec4(a_model_pos,1.0);
	o_tex_coord  = reflect.xyz;
	gl_Position = vec4(a_model_pos.xy, 1.0 , 1.0);
}

);