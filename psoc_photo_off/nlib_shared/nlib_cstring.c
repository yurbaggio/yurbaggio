/*
 *  Created on: Nov 23, 2015
 *      Author: Jianyi
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nlib_assert.h"
#include "nlib_cstring.h"
#include "nlib_print.h"

//constructor, reserve memory for spi transfer
CstringObj cstring_New(unsigned int bufferSize_var){
  CstringObj self = (CstringObj) calloc(1, sizeof(struct cstringObj));
  
  if (self == NULL)
    return NULL;

  //return NULL if we cannot reserve enough memory
  self->ptr = (char *) calloc(bufferSize_var, sizeof(char));
  if (self->ptr == NULL){
    free(self); //allocation not successful
    return NULL;
  }
    
  //size of empty string is now zero
  self->stringSize = 0;
  
  self->bufferSize = bufferSize_var;
  return self;
}

//constructor from array, reserve memory for spi transfer
CstringObj cstring_NewFromArray(const char * src, unsigned int stringSize, unsigned int bufferSize){
  CstringObj self = cstring_New(bufferSize);
  
  if (self == NULL)
    return NULL;
  
  //size of string
  self->stringSize = stringSize;
  
  //copy, check for success
  int status = cstring_CopyFromArray(src, self, stringSize);
  if(status != CSTRING_COPY_SUCCESS){
    cstring_Destroy(self);
    return NULL;
  }
  return self;
}

//destroy a string object
void cstring_Destroy(CstringObj self){
  free(self->ptr);
  free(self);
}

//reset stringSize only, no need to reset buffer
void cstring_ResetStringSize(CstringObj self){
  self->stringSize = 0;
}

//reset buffers, place zeros on buffers
void cstring_Reset(CstringObj self){
  memset(self->ptr, 0, self->bufferSize);
}

//copy cstring
int cstring_Copy(CstringObj src, CstringObj dest){

  //check size
  if (src->stringSize > dest->bufferSize)
    return CSTRING_COPY_ERRSIZE;
  
  //copy
  memcpy(dest->ptr, src->ptr, src->stringSize);
  dest->stringSize = src->stringSize;  
  
  //success
  return CSTRING_COPY_SUCCESS;
}

//copy cstring to/from with index
int cstring_CopyIndex(CstringObj src, int srcIndex, CstringObj dest, int destIndex, unsigned int size){

  //check size source, index + size cannot be smaller than buffersize
  if ((srcIndex + size) > (src->bufferSize))
    return CSTRING_COPY_ERRSRCSIZE;

  //check size dest, index + size cannot be smaller than buffersize
  if ((destIndex + size) > (dest->bufferSize))
    return CSTRING_COPY_ERRDESTSIZE;
  
  //copy
  memcpy(dest->ptr + destIndex, src->ptr + srcIndex, size);
  
  //size should be the largest index touched on dest
  dest->stringSize = destIndex + size + 1;
  
    
  //success
  return CSTRING_COPY_SUCCESS;
}

//copy to cstring, from char array
//make sure size of src is larger or equal to size
int cstring_CopyFromArray(const char * src, CstringObj dest, unsigned int size){
    
  //check size of dest, copy size should be smaller than bufferSize
  if ((size) > (dest->bufferSize))
    return CSTRING_COPY_ERRDESTSIZE;
    
  //change string size
  dest->stringSize = size;
  
  //copy
  memcpy(dest->ptr, src, size);
    
  //success
  return CSTRING_COPY_SUCCESS;
}

//print as hex, 16 per row, the entire buffer
void cstring_PrintBufferAsHexBytes(CstringObj self, int (*va_print)(const char *format, ...)){
  print_CharArrayAsHex(va_print, self->ptr, self->bufferSize, 16);
}

//print as hex, only up to size of string
void cstring_PrintStringAsHexBytes(CstringObj self, int (*va_print)(const char *format, ...)){
  print_CharArrayAsHex(va_print, self->ptr, self->stringSize, 16);
}
