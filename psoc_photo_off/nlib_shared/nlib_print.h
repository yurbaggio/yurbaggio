/******************************************************************************
universal printing
module is useful for printing things
******************************************************************************/

#ifndef NLIB_PRINT_H
#define NLIB_PRINT_H

//prints array as a hex, organized per row
void print_CharArrayAsHex(int (*va_print)(const char *format, ...), const char * src, unsigned int len, unsigned int perrow);

#endif
