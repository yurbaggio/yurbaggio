/******************************************************************************
universal printing
module is useful for printing things
******************************************************************************/

#include "nlib_print.h"

//prints array as a hex, organized per row
void print_CharArrayAsHex(int (*va_print)(const char *format, ...), const char * src, unsigned int len, unsigned int perrow){
  
  //print all, even past stringSize
  unsigned int i;
  for(i = 0; i < len; i ++){
    
    //line header
    if ((i % perrow) == 0)
      va_print("%04x: ", i);
      
    va_print("%02x ", (unsigned char)*(src + i));
    
    //line termination
    if ((i % perrow) == (perrow - 1))
      va_print("\n\r");    
  }
  va_print("\n\r\n\r");
}

