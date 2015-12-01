const char* DepthRTT_vert = STRINGIFY(

in vec3 a_model_pos;

void main()
{
	gl_Position = u_MVP * vec4(a_model_pos,1.0);
}
);