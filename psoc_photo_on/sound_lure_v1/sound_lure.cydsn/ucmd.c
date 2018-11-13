//uart invoked commands
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bsp.h"
#include "ucmd.h"

//delimiters
char DELIMITERS[] = " ,\t";

//escape key detected
void ucmdEscape(const char *str) {
	va_print("Aborting sample sequence\n\r");
}

void ucmdStatus(const char * str) {
	
  //print output of IO pins, VDACs
	va_print("Status Command\n\r");
}

//actuate the LEDs
void ucmdLedTest(const char * str) {
  
  LED_DEVICEON_Write(1);
  CyDelay(10);
  LED_DEVICEON_Write(0);
  CyDelay(1000);
  
  LED_SPKRON_Write(1);
  CyDelay(10);
  LED_SPKRON_Write(0);
  CyDelay(1000);
  
  LED_LOWLIGHT_Write(1);
  CyDelay(10);
  LED_LOWLIGHT_Write(0);
  CyDelay(1000);
  
  LED_LOWBATT_Write(1);
  CyDelay(10);
  LED_LOWBATT_Write(0);
  CyDelay(1000);
}

void ucmdStop(const char * str){

	va_print("Stop Command\n\r");
}

void ucmdSleep(const char * str){
  QEvent *pe = Q_NEW(QEvent, UART_SLEEP_SIG);
  QF_publish(pe);
}

/* [] END OF FILE */