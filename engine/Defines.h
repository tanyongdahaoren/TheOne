#pragma once

#include "MathH.h"

const float sWinW = 1024.0;

const float sWinH = 768.0;

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)

#define BREAK_IF(cond)           if(cond) break

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

//#define STRINGIFY(A)  #A