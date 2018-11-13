//uart invoked commands
#ifndef UCMD_H
#define UCMD_H

//escape key detected
void ucmdEscape(const char *str);

//test commands
void ucmdStatus(const char * str);
void ucmdStop(const char * str);

//actuate the LEDs
void ucmdLedTest(const char * str);

//go to deep sleep
void ucmdSleep(const char * str);

#endif

/* [] END OF FILE */
