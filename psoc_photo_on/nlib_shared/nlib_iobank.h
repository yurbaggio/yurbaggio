//iobank lookup table and manager
#ifndef NLIB_SYS_IOBANK_H
#define NLIB_SYS_IOBANK_H
    
#include "nlib_cstring.h"

#define IOBANK_LABEL_MAX 16 //max length of io pin label
#define IO_IN   0
#define IO_OUT  1
    
//create a io object, these objects will be registered into a master list
typedef struct IoPinStruct{
    void *next;     //pointer to next io structure
    void (*ioWrite)(unsigned char val);
    unsigned char (*ioRead)(void);
    unsigned char (*ioReadDataReg)(void);
    int ioDir;
    CstringObj ioLabel;
}IoPin;

//module object
typedef struct iobankObj * IobankObj;

struct iobankObj{
    int ioCount;
    IoPin *head;   //starting pointer to list of ios
    IoPin *tail;
    
    int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
};

//constructor
IobankObj iobank_New();

//initialize iobank
int iobank_Init(IobankObj self, int (*va_print_var)(const char *, ...));

//checks to see if io label exists in the list of labels
//returns pointer to object
//returns null if it does not exist or invalid label
IoPin * iobank_CheckPin(IobankObj self, const char * label);

//add an IO
//whitespace not allowed on label
int iobank_AddIn(IobankObj self, const char * label, unsigned char (*ioRead)(void));
int iobank_AddOut(IobankObj self, const char * label, void (*ioWrite)(unsigned char val),  unsigned char (*ioReadDataReg)(void));

//print io status for all pins
void iobank_ucmdIoStatus(IobankObj self);

//change an output IO
//returns the relevant pointer to IO objet, if found, NULL otherwise
IoPin * iobank_Set(IobankObj self, const char * label, unsigned char val);

//change an io output
//returns the relevant pointer to IO objet, if found, NULL otherwise
IoPin * iobank_ucmdIoSet(IobankObj self, const char * str);

#endif
