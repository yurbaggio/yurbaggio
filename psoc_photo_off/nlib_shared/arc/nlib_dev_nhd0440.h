/******************************************************************************
NVIS Inc.
Jianyi Liu
Created		Jun 19, 2012
Modified	Jun 19, 2012
Newhaven Display International Model NHD-0440 LCD display driver
******************************************************************************/

#ifndef NLIB_DEV_NHD0440_H
#define NLIB_DEV_NHD0440_H

//define lcd commands
#define NHD0440_CMD_CLEAR		0b00000001
#define NHD0440_CMD_RETURN_HOME	0b00000010
#define NHD0440_CMD_DISPLAY_ON	0b00001100
#define NHD0440_CMD_DISPLAY_OFF	0b00001000
#define NHD0440_CMD_FSET_LN2_FS8	0b00111000
#define NHD0440_CMD_FSET_LN2_FS11	0b00111001
#define NHD0440_CMD_ENTRY_INC		0b00000110
#define NHD0440_CMD_LINE0		(0x80)
#define NHD0440_CMD_LINE1		(0x80|0x28)

//describe the LCD
#define NHD0440_LINE_SIZE	40
#define NHD0440_LINE_COUNT	4

typedef struct nhd0440Obj * Nhd0440Obj;

//define a nhd0440 base function interface
typedef struct Nhd0440FuncStruct{
	void (*lcd_db0)(int);
	void (*lcd_db1)(int);
	void (*lcd_db2)(int);
	void (*lcd_db3)(int);
	void (*lcd_db4)(int);
	void (*lcd_db5)(int);
	void (*lcd_db6)(int);
	void (*lcd_db7)(int);
	void (*lcd_e1)(int);
	void (*lcd_e2)(int);
	void (*lcd_rw)(int);
	void (*lcd_rs)(int);
}Nhd0440Func;

//define nhd0440 object
struct nhd0440Obj{
	Nhd0440Func		fTable;
};

//constructor, resrve memory
Nhd0440Obj nhd0440_New();

//initialization
int nhd0440_Init(Nhd0440Obj self, Nhd0440Func fTable_var);

//set position of cursor
void nhd0440_SetPosition(Nhd0440Obj self, unsigned int x, unsigned int y);

//clear the whole display
void nhd0440_CmdTop(Nhd0440Obj self, unsigned char cmd);
void nhd0440_CmdBot(Nhd0440Obj self, unsigned char cmd);

//print to lcd on absolute line number
void nhd0440_PrintLine(Nhd0440Obj self, unsigned int line, const char *message);

//print to lcd on absolute line number, centered
void nhd0440_PrintLineCenter(Nhd0440Obj self, unsigned int line, const char *message);

#ifdef qp_port_h
//an LCD printout type event
typedef struct LcdLineEvtTag {
	QEvent super;
	char message[NHD0440_LINE_SIZE + 1];
} LcdLineEvt;

//an LCD printout type event
typedef struct LcdCharEvtTag {
	QEvent super;
	char c;
} LcdCharEvt;
#endif

#endif
