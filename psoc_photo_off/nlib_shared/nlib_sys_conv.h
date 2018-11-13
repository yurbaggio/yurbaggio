/******************************************************************************
NVIS Inc.
Jianyi Liu
Created    Mar 30, 2017
Modified  Mar 30, 2010
Override the interrupt routine in the library
Macros for various stuff
******************************************************************************/

#ifndef nlib_sys_conv_h
#define nlib_sys_conv_h

#include <stdlib.h>
#include <limits.h>

//get sign
#define SIGN(x) ((x < 0) ? -1 : 1)    //returns sign of value, 0 considered positive

//range macros
#define RANGE(i, min, max) (i >= min) && (i <= max) ? 1 : 0     //returns if it is within range
#define RANGEINC(i, max) ((i+1) <= max) ? i+1 : i               //increment only if within range
#define RANGEDEC(i, min) ((i-1) >= min) ? i-1 : i               //decrement only if within range

//limit macros
#define LIMH(i, lim) ((i > lim) ? lim : i)                                //returns higher limit, or value
#define LIML(i, lim) ((i < lim) ? lim : i)                                //returns lower limit, or value
#define LIM(i, llim, hlim) ((i > hlim) ? hlim : ((i < llim) ? llim : i))  //returns higher or lower limit, or value if within limit

//float macros
#define FLOAT2INT(f) (int)((f < 0.0f) ? f - 0.5f : f + 0.5f)    //round float to nearest integer

//conversion macros
#define LONGLO(X) (unsigned int)((unsigned long)(X)&0x0000ffff)         //returns lower short from 32 bit
#define LONGHI(X) (unsigned int)(((unsigned long)(X)>>16)&0x0000ffff)   //returns higher short from 32 bit

#define LONGLL(X) (unsigned char)((unsigned long)(X)&0x000000ff)        //returns low low byte from 32 bit
#define LONGLH(X) (unsigned char)(((unsigned long)(X)>>8)&0x000000ff)   //returns low high byte from 32 bit
#define LONGHL(X) (unsigned char)(((unsigned long)(X)>>16)&0x000000ff)  //returns high low byte from 32 bit
#define LONGHH(X) (unsigned char)(((unsigned long)(X)>>24)&0x000000ff)  //returns high high byte from 32 bit

#define SHORTLO(X) (unsigned char)(((unsigned short)(X))&0x00ff)      //returns lower byte from 16 bit
#define SHORTHI(X) (unsigned char)(((unsigned short)(X)>>8)&0x00ff)   //returns higher byte from 16 bit

#define CHARLO(X) (unsigned char)(((unsigned char)(X))&0x0f)          //returns lower nibble from 8 bit
#define CHARHI(X) (unsigned char)(((unsigned char)(X)>>4)&0x0f)       //returns higher nibble from 8 bit

#define NIB2CHAR(high, low) (unsigned char)((((unsigned char)(high & 0x0f)) << 4 ) + ((unsigned char)(low & 0x0f)))                       //combine 2 nibbles into a char

#define CHAR2SHORT(high, low) (unsigned short)(((unsigned short)(0xFF & high) << 8) + (unsigned short)(0xFF & low))                                           //combine 2 char into short
#define CHAR2LONG(hh, hl, lh, ll) (unsigned long)(((unsigned char)(0xFF & hh) << 24) + ((unsigned char)(0xFF & hl) << 16) + ((unsigned char)(0xFF & lh) << 8) + (unsigned char)(0xFF & ll))  //combine 4 char into long

//other useful conversions
#define INT2BCDCHAR(i) (char)(((i)/10 << 4) + (i)%10)       //integer to BCD
#define BCDCHAR2INT(i) (int)((((i) >> 4)*10) + ((i)&0x0F))  //BCD to integer

//get sign, whole integer, fractional integer
#define INT2SIGN(x)			((x < 0)?'-':' ')
#define INT2WHOLE(x, div)	(abs(x) / div)
#define INT2FRAC1(x, div)	(((abs(x) % div)*10)/div)
#define INT2FRAC2(x, div)	(((abs(x) % div)*100)/div)
#define INT2FRAC3(x, div)	(((abs(x) % div)*1000)/div)
#define INT2FRAC4(x, div)	(((abs(x) % div)*10000)/div)
#define INT2FRAC5(x, div)	(((abs(x) % div)*100000)/div)
#define INT2FRAC6(x, div)	(((abs(x) % div)*1000000)/div)

//extend sign for a certain bit number
//x is value to extend, bit is number of bits, with MSB representing sign
#define EXSIGN_LONG(x, bit)  (((x) & (1 << ((bit) - 1))) ? ((ULONG_MAX - ((1 << (bit)) - 1)) | (x)) : (x))

#endif
