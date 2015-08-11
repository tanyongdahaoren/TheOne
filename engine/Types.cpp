#include "Types.h"

/**
* Size constants
*/
Size::Size()
	: w(0)
	, h(0)
{}
Size::Size(float _w, float _h)
	: w(_w)
	, h(_h)
{}


/**
* Color3B constants
*/

const Color3B Color3B::BLACK(0, 0, 0);
const Color3B Color3B::WHITE(255, 255, 255);
const Color3B Color3B::RED(255, 0, 0);
const Color3B Color3B::GREEN(0, 255, 0);
const Color3B Color3B::BLUE(0, 0, 255);
const Color3B Color3B::YELLOW(255, 255, 0);

Color3B::Color3B()
	: r(0)
	, g(0)
	, b(0)
{}

Color3B::Color3B(GLubyte _r, GLubyte _g, GLubyte _b)
	: r(_r)
	, g(_g)
	, b(_b)
{}

/**
* Color3F constants
*/

const Color3F Color3F::BLACK(0, 0, 0);
const Color3F Color3F::WHITE(1, 1, 1);
const Color3F Color3F::YELLOW(1, 1, 0);
const Color3F Color3F::GREEN(0, 1, 0);
const Color3F Color3F::BLUE(0, 0, 1);
const Color3F Color3F::RED(1, 0, 0);

Color3F::Color3F()
	: r(0)
	, g(0)
	, b(0)
{}

Color3F::Color3F(GLfloat _r, GLfloat _g, GLfloat _b)
	: r(_r)
	, g(_g)
	, b(_b)
{}
