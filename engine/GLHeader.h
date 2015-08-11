#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <GL/glu.h>

extern GLEWContext _glewctx;
#define glewGetContext() (&_glewctx)  

// Include GLFW
#include <glfw3.h>