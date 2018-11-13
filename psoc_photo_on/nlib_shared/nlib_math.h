/******************************************************************************
Engscope.com
Jianyi Liu
Created		Dec 25, 2013
Modified	Dec 25, 2013
fast sine calculations
******************************************************************************/

#ifndef NLIB_MATH_H
#define NLIB_MATH_H

//calculate integer square root, fast
unsigned int isqrt32(unsigned long n);

float nt_fsin(float x);
float nt_fpsin(float x);
float nt_saw(float x);
float nt_radmod(float x);

#define NT_PIQUARTER (0.78539816)
#define NT_PIHALF	 (1.57079633)
#define NT_PI		 (3.14159265)
#define NT_2PI		 (6.28318531)
#define NT_PIINV	 (0.318309886)	// 1/PI

#endif
