//AD9838 DDS drivers
#ifndef NLIB_DEV_AD9838_H
#define NLIB_DEV_AD9838_H

#include <cytypes.h>

typedef struct ad9838Obj * Ad9838Obj;
    
//define a i2c function table
typedef struct Ad9838FuncStruct{
    uint8 (*spiOutWrite)(uint16 data, uint8 mask);	//base function table
    uint8 (*spiOutDone)(void);  //is the transaction done?
}Ad9838Func;
    
struct ad9838Obj{
    Ad9838Func	fTable;	//base function table
    long mclk;   //MCLK frequency into the DDS
    unsigned char mask;  //this is the channel number for this object, used to select the proper SS signal
    
    double freqHz;  //last loaded frequency
    float phaseRad;    //last loaded phase
    unsigned int option;    //last loaded option
    
    int (*va_print)(const char *format, ...);  //link this to either vprintf or vsprintf
};

//constructor
Ad9838Obj ad9838_New();

//freq must be smaller than AD9838_FREQ_MAX
//initialize ad9838
int ad9838_Init(Ad9838Obj self, Ad9838Func fTable, long mclk, unsigned char mask, int (*va_print)(const char *format, ...));

//set dds
//frequency in Hz, phase in radians
//determines signal shape
void ad9838_SetDds(Ad9838Obj self, double freqHz, float phaseRad, unsigned int option);

//sleep Dds
void ad9838_SleepDds(Ad9838Obj self);

//reset Dds
void ad9838_SwResetDdsSq2(Ad9838Obj self);

//ucmd wrapper for set DDS
void ad9838_ucmdSetDds(Ad9838Obj self, const char * str);

#endif

/* [] END OF FILE */
