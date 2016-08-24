#ifndef _MATH_EX
#define _MATH_EX

#include <math.h>

float fabsf(float f);
#if 0
{
	if(f<0)
		return (float)(0-f);
	else
		return (float)(f);
}
#endif

float atan2f(float f1, float f2);
#if 0
{
	return (float)(atan2(f1,f2));
}
#endif

float cosf(float f);
#if 0
{
	return (float)cos(f);
}
#endif

float sinf(float f);
#if 0
{
	return (float)sin(f);
}
#endif

float sqrtf(float f);
#if 0
{
	return (float)sqrt(f);
}
#endif

#endif// _MATH_EX 
