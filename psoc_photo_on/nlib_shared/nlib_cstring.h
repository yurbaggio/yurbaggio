/*
 *  Created on: Nov 26, 2015
 *      Author: Jianyi
 */

#ifndef NLIB_CSTRING_H_
#define NLIB_CSTRING_H_

#define CSTRING_COPY_SUCCESS		0
#define CSTRING_COPY_ERRSIZE		-1
#define CSTRING_COPY_ERRSRCSIZE		-3
#define CSTRING_COPY_ERRDESTSIZE	-4

//spi transfer object
typedef struct cstringObj * CstringObj;
    
struct cstringObj{
  char * ptr;
  unsigned int stringSize;
  unsigned int bufferSize;
};

//constructor, reserve memory for spi transfer
CstringObj cstring_New(unsigned int bufferSize);

//constructor from array, reserve memory for spi transfer
CstringObj cstring_NewFromArray(const char * src, unsigned int stringSize, unsigned int bufferSize);

//destroy a string object
void cstring_Destroy(CstringObj self);

//reset stringSize only, no need to reset buffer
void cstring_ResetStringSize(CstringObj self);

//reset buffers, place zeros on buffers
void cstring_Reset(CstringObj self);

//copy cstring
int cstring_Copy(CstringObj src, CstringObj dest);

//copy cstring to/from with index
int cstring_CopyIndex(CstringObj src, int srcIndex, CstringObj dest, int destIndex, unsigned int size);

//copy to cstring, from char array
int cstring_CopyFromArray(const char * src, CstringObj dest,  unsigned int size);

//print as hex, 16 per row, the entire buffer
void cstring_PrintBufferAsHexBytes(CstringObj self, int (*va_print)(const char *format, ...));

//print as hex, only up to size of string
void cstring_PrintStringAsHexBytes(CstringObj self, int (*va_print)(const char *format, ...));

#endif
