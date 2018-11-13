//iobank lookup table and manager

#include <stdlib.h>
#include <string.h>
#include "nlib_err.h"
#include "nlib_cproc.h"
#include "nlib_iobank.h"

//constructor
IobankObj iobank_New(){
    IobankObj self = (IobankObj) calloc(1, sizeof(struct iobankObj));
	return self;
}

//initialize iobank
int iobank_Init(IobankObj self, int (*va_print_var)(const char *, ...)){
    
    if (self == NULL)
	    return NLIB_ERR_NOMEM;
    
    //connect interrupt enable/disable
	self->va_print = va_print_var;
    
    //reset head, count
    self->head = NULL;
    self->tail = NULL;
    self->ioCount = 0;
    
    return NLIB_ERR_NOERROR;
}

//checks to see if io label exists in the list of labels
//returns pointer to object
//returns null if it does not exist or invalid label
IoPin * iobank_CheckPin(IobankObj self, const char * label) {
    
    //check string length
    if (strlen(label) > 16)
        return NULL;
    
    IoPin * pin = self->head;
    while(pin != NULL){
        if (strcmp(label, pin->ioLabel->ptr) == 0)
            return pin;
        pin = pin->next;
    }
    
    //label does not exist
    return NULL;
}

//add an IO
//whitespace not allowed on label
int iobank_Add(IobankObj self, const char * label, void (*ioWrite)(unsigned char val), unsigned char (*ioRead)(void), unsigned char (*ioReadDataReg)(void), int direction){
    
    //check string length
    if (strlen(label) > 16)
        return NLIB_ERR_INVALPARAM;
    
    //check existing labels
    IoPin * exist = iobank_CheckPin(self, label);
    
    //label already exists, cannot add
    if (exist != NULL)
        return -1;
    
    //if still possible to add, create an IoPin instance, reserve memory, then add to the linked list
    IoPin * pin = (IoPin *) calloc(1, sizeof(IoPin));
    
    //reserve label memory, then copy the string
    pin->ioLabel = cstring_New(IOBANK_LABEL_MAX + 1);
    if (pin->ioLabel == NULL)
        return NLIB_ERR_NOMEM;
    strcpy(pin->ioLabel->ptr, label);
    
    //copy function calls
    pin->ioWrite = ioWrite;
    pin->ioReadDataReg = ioReadDataReg;
    pin->ioRead = ioRead;
    pin->next = NULL;
    pin->ioDir = direction;
    
    //assign linked list pointers
    //if first assignment, head and tail would be null
    if (self->head == NULL && self->tail == NULL){
        self->head = pin;
        self->tail = pin;
        
    //make sure to link the list before assignment to tail
    }else{
        self->tail->next = pin;
        self->tail = pin;
    }
    
    //increment number of Ios
    self->ioCount ++;
    
    return NLIB_ERR_NOERROR;
}

//add an input
int iobank_AddIn(IobankObj self, const char * label, unsigned char (*ioRead)(void)){
    return iobank_Add(self, label, NULL, ioRead, NULL, IO_IN);
}

//add an output
int iobank_AddOut(IobankObj self, const char * label, void (*ioWrite)(unsigned char val),  unsigned char (*ioReadDataReg)(void)){
    return iobank_Add(self, label, ioWrite, NULL, ioReadDataReg, IO_OUT);
}

//print io status for all pins
void iobank_ucmdIoStatus(IobankObj self){

    //print header
    self->va_print("Listing io status\n\r");
    
    //check null
    if (self->head == NULL)
        self->va_print("No labels are registered\n\r");
    
    //otherwise, loop through entire linked list, read value
    IoPin * pin = self->head;
    while(pin != NULL){
        if(pin->ioDir == IO_OUT)
            self->va_print("%5s%20s:  %d\n\r", "OUT", pin->ioLabel->ptr, pin->ioReadDataReg());
        else
            self->va_print("%5s%20s:  %d\n\r", "IN", pin->ioLabel->ptr, pin->ioRead());
            
        pin = pin->next;
    }
    
    //closing footer
    self->va_print("\n\r");
}

//change an output IO
//returns the relevant pointer to IO objet, if found, NULL otherwise
IoPin * iobank_Set(IobankObj self, const char * label, unsigned char val){
    
    //find the pin
    IoPin * exist = iobank_CheckPin(self, label);
    
    //if not found
    if (exist == NULL){
        self->va_print("No io with label %s found in iobank registry, no changes applied\n\r\n\r", label);
        return NULL;
    }
     
    //if found, check direction
    if(exist->ioDir == IO_IN){
        self->va_print("Input io %s cannot be written to, no changes applied\n\r\n\r", label);
        return NULL;
    }
    
    //change GPIO
    exist->ioWrite(val);
    self->va_print("Io %s found, writing value of %d\n\r\n\r", label, 0x1&val);
    
    return exist;
}

//change an io output
//returns the relevant pointer to IO objet, if found, NULL otherwise
IoPin * iobank_ucmdIoSet(IobankObj self, const char * str){
    
    //copy string, since tokenizer will modify the string
	char cpy[CPROC_LINEBUFF_SIZE + 1];
	strcpy(cpy, str);
    
    //tokenize, discard first since it is the command itself, create test pointer for strtol/strtof
	char *pch = strtok(cpy, CPROC_DELIMITERS);
    char *strTest;
    
    //2nd token is label string
    char *strLabel = strtok(NULL, CPROC_DELIMITERS);
    
    //3rd token is write value
    pch = strtok(NULL, CPROC_DELIMITERS);
    unsigned char val = (unsigned char)strtol(pch, &strTest, 10);
    if (strTest == pch){
        self->va_print("Need value to write to io, 1 or 0\n\r\n\r");
        return NULL;
    }
    if ((val > 1) || (val < 0)){
        self->va_print("Value to be written must be 1 or 0\n\r\n\r");
        return NULL;
    }
        
    return iobank_Set(self, strLabel, val);
}
