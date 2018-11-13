//fast square root function
//from: http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

//fast trig functions, use parabola to estimate sine
//from http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine

//add library m to use fmod

#include <math.h>
#include "nlib_sys_conv.h"
#include "nlib_math.h"

//calculate integer square root, fast
unsigned int isqrt32(unsigned long n)
{  
    unsigned int c = 0x8000;  
    unsigned int g = 0x8000;  
  
    for(;;) {  
        if(g*g > n)  
            g ^= c;  
        c >>= 1;  
        if(c == 0)  
            return g;  
        g |= c;  
    }  
}  

//for fast sine, average error 0.056
//use parabola 4/pi x + 4/pi^2 x^2
//allows range -pi to pi
const float B = 1.273239;	//	4/pi
const float C = -0.405284;	//	-4/(pi*pi)
float nt_fsin(float x){
	return B * x + C * x * fabs(x);
}

//for fast precise sine, need to boost the "shoulders", average error 0.001
//Q (4/pi x - 4/pi^2 x^2) + P (4/pi x - 4/pi^2 x^2)^2
//with Q + P = 1
//optimize error for Q and P
//Q = 0.775, P = 0.225	for smallest absolute error
//Q = 0.782, P = 0.218	for smallest relative error
const float P = 0.225;
float nt_fpsin(float x){
	float y = nt_fsin(x);
	return P * (y * fabs(y) - y) + y;	// Q * y + P * y * abs(y)
}

//calculate saw tooth
float nt_saw(float x){
	
	if (fabs(x) < NT_PIHALF)
		return 2 * x * NT_PIINV;
	else
		return 2 * SIGN(x) - (2 * x * NT_PIINV);
}

//add angles, roll over if needed
//since fast sine only calculates from -pi to pi,
float nt_radmod(float x){
	if (x > 0)
		return fmod(x + NT_PI, NT_2PI) - NT_PI;
	else
		return fmod(x - NT_PI, NT_2PI) + NT_PI;
}
