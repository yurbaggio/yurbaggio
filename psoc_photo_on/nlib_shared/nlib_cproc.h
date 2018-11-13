/******************************************************************************
Jianyi Liu
Created		May 30, 2015
Modified	May 30, 2015
Command processing module
******************************************************************************/

#ifndef NLIB_CPROC_H
#define NLIB_CPROC_H

#include "nlib_cstring.h"
#include "nlib_cstring_circbuf.h"
    
#define CPROC_CMDCOUNT_MAX 32				//max number of commands that can be stored in the module
#define CPROC_CMDLENGTH_MAX 16 + 1			//max number of characters per command

//default is 100 characters per command line, but can be something else if needed for more/less memory
#ifndef CPROC_LINEBUFF_SIZE
  #define CPROC_LINEBUFF_SIZE 100
#endif

#define CPROC_CHARBUFF_SIZE 1
#define CPROC_MAXSEQ_SIZE 3                 //max escape sequence
#define CRPOC_ESCAPE_COUNT_LIMIT 4

//default delimiters
static const char CPROC_DELIMITERS[] = " ,\t";
    
//Command object, encapsulates a command string and function pointer
typedef struct CprocCmdStruct{
	char cmdString[CPROC_CMDLENGTH_MAX];
	void (*cmdFunction)(const char * cmdArgStr);
}CprocCmd;

//Module object
typedef struct cprocObj * CprocObj;

struct cprocObj{
	int cmdCount;								//number of commands added to module
	char * delimiters;
	CprocCmd * (cmdEscape);						//escape command, when escape character is pressed, execute this command to escape loop
	CprocCmd * (cmdList[CPROC_CMDCOUNT_MAX]);	//command object, contains function to invoke as well as
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
CprocObj cproc_New();

//initializes command processor, one terminal only, va_print_src and va_print are the same
int cproc_Init(CprocObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize);

//initializes command processor, allows for multi terminal function
//takes the command processor, a printf function, a receive 1 character from incoming buffer function, delimiters string ponter
int cproc_InitMultiTerm(CprocObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_src_var)(const char *, ...), int (*va_print_var)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize);

//checks to see if command exists in the list of commands
//returns index of the command in the list
int cproc_CheckCommand(CprocObj self, const char * cmd);

//add escape command
int cproc_AddEscape(CprocObj self, void (*cmdEscape)(const char * cmdStr));

//add a command
int cproc_Add(CprocObj self, const char * cmd, void (*cmdInvoke)(const char * cmdStr));

//invoke a command
void cproc_Invoke(CprocObj self, const char * str);

//flush current buffers clear the current line
void cproc_Flush(CprocObj self);

//process events
void cproc_ProcessEvents(CprocObj self);

#endif
