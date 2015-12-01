const char* DepthRTT_frag = STRINGIFY(

out float o_fragmentdepth;

void main()
{
	o_fragmentdepth = gl_FragCoord.z;
}
);