/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jun 19, 2012
Modified	Jun 19, 2012
Newhaven Display International Model NHD-0440 LCD display driver
******************************************************************************/

//uses calloc, must include stdlib
#include <stdlib.h>
#include <string.h>

#include "nlib_sys_err.h"
#include "nlib_dev_nhd0440.h"

//constructor, resrve memory
Nhd0440Obj nhd0440_New(){
	Nhd0440Obj self = (Nhd0440Obj)calloc(1, sizeof(struct nhd0440Obj));
	return self;
}

//initialization
int nhd0440_Init(Nhd0440Obj self, Nhd0440Func fTable_var){
	if(self == NULL)
		return NLIB_ERR_NOMEM;

	//initiate values
	self->fTable	= fTable_var;
	
	//reset enable
	self->fTable.lcd_e1(0);
	self->fTable.lcd_e2(0);
	
	return NLIB_ERR_NOERROR;
}

//small delay
void nhd0440_smallDelay(){
	unsigned int i;
	//for(i = 0; i < 1480; i ++);
	for(i = 0; i < 320; i ++);
}

//large delay
void nhd0440_largeDelay(){
	unsigned int i;
	//for(i = 0; i < 60800; i ++);
	for(i = 0; i < 15200; i ++);
}

//set data
void nhd0440_setData(Nhd0440Obj self, unsigned char c){
	self->fTable.lcd_db0((c & 0x1<<0)>>0);
	self->fTable.lcd_db1((c & 0x1<<1)>>1);
	self->fTable.lcd_db2((c & 0x1<<2)>>2);
	self->fTable.lcd_db3((c & 0x1<<3)>>3);
	self->fTable.lcd_db4((c & 0x1<<4)>>4);
	self->fTable.lcd_db5((c & 0x1<<5)>>5);
	self->fTable.lcd_db6((c & 0x1<<6)>>6);
	self->fTable.lcd_db7((c & 0x1<<7)>>7);
}

//send command to display
void nhd0440_CmdTop(Nhd0440Obj self, unsigned char cmd){
	//set cmd/data
	self->fTable.lcd_rs(0);
	
	//set read/write
	self->fTable.lcd_rw(0);
	
	//reset E1, E2
	self->fTable.lcd_e1(0);

	//set clear command
	nhd0440_setData(self, cmd);

	//toggle enable
	self->fTable.lcd_e1(1);
	
	//delay
	nhd0440_largeDelay();
	
	//reset enable
	self->fTable.lcd_e1(0);

	//add small delay
	nhd0440_smallDelay();
}

void nhd0440_CmdBot(Nhd0440Obj self, unsigned char cmd){
	//set cmd/data
	self->fTable.lcd_rs(0);

	//set read/write
	self->fTable.lcd_rw(0);

	//reset E1, E2
	self->fTable.lcd_e2(0);

	//set clear command
	nhd0440_setData(self, cmd);

	//toggle enable
	self->fTable.lcd_e2(1);

	//delay
	nhd0440_largeDelay();

	//reset enable
	self->fTable.lcd_e2(0);

	//add small delay
	nhd0440_smallDelay();
}

//print to lcd on absolute line number
void nhd0440_PrintLine(Nhd0440Obj self, unsigned int line, const char *message){

	//get line ot determine E1/E2
	void (*lcd_e)(int);
	switch(line){
		default:
			nhd0440_CmdTop(self, NHD0440_CMD_LINE0);
			lcd_e = self->fTable.lcd_e1;
			break;
		case 1:
			nhd0440_CmdTop(self, NHD0440_CMD_LINE1);
			lcd_e = self->fTable.lcd_e1;
			break;
		case 2:
			nhd0440_CmdBot(self, NHD0440_CMD_LINE0);
			lcd_e = self->fTable.lcd_e2;
			break;
		case 3:
			nhd0440_CmdBot(self, NHD0440_CMD_LINE1);
			lcd_e = self->fTable.lcd_e2;
			break;
	}

	//set cmd/data
	self->fTable.lcd_rs(1);

	//set read/write
	self->fTable.lcd_rw(0);

	//reset E1, E2
	self->fTable.lcd_e1(0);
	self->fTable.lcd_e2(0);
	
	//process message, process 40 characters.  If null found then use space
	int i;
	for(i = 0; (i < NHD0440_LINE_SIZE); i++){

		//test to see if null has been reached
		if (*message != '\0'){
			nhd0440_setData(self, *message);
			message++;
		}else
			nhd0440_setData(self, ' ');
		
		//set enable
		lcd_e(1);
		
		//delay
		nhd0440_smallDelay();
		
		//reset enable
		lcd_e(0);

		//add small delay
		nhd0440_smallDelay();
	}
}

//justify center on 40 character LCD
void nhd0440_PrintLineCenter(Nhd0440Obj self, unsigned int line, const char *message){

	//first find length, then send to print line
	int len = strlen(message);

	//no string
	if(len == 0)
		return;

	//at 39 or 40 characters, no need to pad
	if(len >= NHD0440_LINE_SIZE - 1){
		nhd0440_PrintLine(self, line, message);
		return;
	}

	//in between, calculat how many characters to pad with space
	int pad = (NHD0440_LINE_SIZE - len)/2;
	char padChar[NHD0440_LINE_SIZE + 1];

	//pad the beginning of the string
	int i;
	for (i = 0; i < pad; i ++)
		padChar[i] = ' ';
	padChar[pad] = '\0';

	//copy message to end of string, send to print
	strcat(padChar, message);
	nhd0440_PrintLine(self, line, padChar);
}
