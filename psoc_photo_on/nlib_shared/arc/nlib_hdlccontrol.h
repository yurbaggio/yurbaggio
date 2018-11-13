/******************************************************************************
hdlc controller
uses yahdlc library
******************************************************************************/

#ifndef NLIB_HDLCCONTROL_H
#define NLIB_HDLCCONTROL_H

//default delimiters
static const char CPROC_DELIMITERS[] = " ,\t";
    
//Module object
typedef struct hdlcObj * HdlcObj;

struct hdlcObj{

  txControl;

	int cmdCount;								//number of commands added to module
	char * delimiters;
	HdlcCmd * (cmdEscape);						//escape command, when escape character is pressed, execute this command to escape loop
	HdlcCmd * (cmdList[CPROC_CMDCOUNT_MAX]);	//command object, contains function to invoke as well as
	int (*va_print)(const char *, ...);         //print function, send back to all terminals
    int (*va_print_src)(const char *, ...);     //print function, send back to incoming terminal only, source of command
	unsigned int (*p_recv)(char *BufferPtr);	//receive buffer function, receives 1 character, store in buffer, return number received

	int squeue_msg_index;
	char squeue_msg[CPROC_LINEBUFF_SIZE + 1];	//store message as it comes
	char cqueue_msg[CPROC_LINEBUFF_SIZE + 1];	//stores the full message after '/r' character
    
    CircbufObj cmdBuffer;
    CstringObj fetch;
    CstringObj spare;
    int cmdBufferIndex;
    
    char seqBuf[CPROC_MAXSEQ_SIZE + 1];
    int escWaitCount;
};

//constructor, reserve memory
HdlcObj hdlc_New();

//initializes command processor, one terminal only, va_print_src and va_print are the same
int hdlc_Init(HdlcObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize);

//initializes command processor, allows for multi terminal function
//takes the command processor, a printf function, a receive 1 character from incoming buffer function, delimiters string ponter
int hdlc_InitMultiTerm(HdlcObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_src_var)(const char *, ...), int (*va_print_var)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize);

//checks to see if command exists in the list of commands
//returns index of the command in the list
int hdlc_CheckCommand(HdlcObj self, const char * cmd);

//add escape command
int hdlc_AddEscape(HdlcObj self, void (*cmdEscape)(const char * cmdStr));

//add a command
int hdlc_Add(HdlcObj self, const char * cmd, void (*cmdInvoke)(const char * cmdStr));

//invoke a command
void hdlc_Invoke(HdlcObj self, const char * str);

//process events
void hdlc_ProcessEvents(HdlcObj self);

#endif
