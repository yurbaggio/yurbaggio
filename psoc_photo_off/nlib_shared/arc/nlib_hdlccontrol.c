/******************************************************************************
hdlc controller
uses yahdlc library
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nlib_err.h"
#include "nlib_hdlccontrol.h"

//constructor, reserve memory
HdlcObj hdlc_New() {
	HdlcObj self = (HdlcObj) calloc(1, sizeof(struct hdlcObj));
	return self;
}

//initializes command processor, one terminal only, va_print_src and va_print are the same
int hdlc_Init(HdlcObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize){
    return hdlc_InitMultiTerm(self, enInt_var, disInt_var, va_print, va_print, p_recv_var, delimiters_var, queueSize);
}

//initializes command processor, allows for multi terminal function
//takes the command processor, a printf function, a receive 1 character from incoming buffer function, delimiters string ponter
int hdlc_InitMultiTerm(HdlcObj self, void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_src_var)(const char *, ...), int (*va_print_var)(const char *, ...), unsigned int (*p_recv_var)(char *BufferPtr), char * delimiters_var, unsigned int queueSize){
    if (self == NULL)
		return NLIB_ERR_NOMEM;

	//assign pointers for printing and receiving characters
    self->va_print_src = va_print_src_var;
	self->va_print = va_print_var;
	self->p_recv = p_recv_var;
	self->delimiters = delimiters_var;

	self->cmdCount = 0;

	//test to see if memory is well reserved
    self->cmdBuffer = circbuf_New();
	int status = circbuf_Init(self->cmdBuffer, enInt_var, disInt_var, va_print_var, CPROC_LINEBUFF_SIZE + 1, queueSize);
	if (status != NLIB_ERR_NOERROR)
		return status;
	
    self->fetch = cstring_New(CPROC_LINEBUFF_SIZE + 1);
    if (self->fetch == NULL)
        return NLIB_ERR_NOMEM;
 
    self->spare = cstring_New(CPROC_LINEBUFF_SIZE + 1);
    if (self->spare == NULL)
        return NLIB_ERR_NOMEM;
    
    self->cmdBufferIndex = -1;
    
	return NLIB_ERR_NOERROR;
}



//checks to see if command exists in the list of commands
//returns index of the command in the list
int hdlc_CheckCommand(HdlcObj self, const char * cmd) {

	//go through list, see if command matches
	int i;
	for (i = 0; i < self->cmdCount; i++) {
		if (strcmp(cmd, self->cmdList[i]->cmdString) == 0)
			return i;
	}

	//if we got this far, no command found
	return -1;
}

//add escape
int hdlc_AddEscape(HdlcObj self, void (*cmdEscape)(const char * cmdStr)) {
	self->cmdEscape = (HdlcCmd *) calloc(1, sizeof(HdlcCmd));
	self->cmdEscape->cmdFunction = cmdEscape;

	return 0;
}

//add a command
int hdlc_Add(HdlcObj self, const char * cmd, void (*cmdInvoke)(const char * cmdStr)) {

	//check existing commands
	int exist = hdlc_CheckCommand(self, cmd);

	//cannot add, command already exists
	if (exist >= 0)
		return -1;

    //check command count
    if(self->cmdCount + 1 > CPROC_CMDCOUNT_MAX)
        return -2;
    
	//reserve memory add the command to the list
	self->cmdList[self->cmdCount] = (HdlcCmd *) calloc(1, sizeof(HdlcCmd));
	self->cmdList[self->cmdCount]->cmdFunction = cmdInvoke;
	strcpy(self->cmdList[self->cmdCount]->cmdString, cmd);

	//increment command count list
	self->cmdCount++;

	return 0;
}

//invoke a command
void hdlc_Invoke(HdlcObj self, const char * str){
    
    //print command
	char buffer[CPROC_LINEBUFF_SIZE];
	sprintf(buffer, "%s\r\n", str);
	self->va_print(buffer);

	//check null
	if (str[0] == '\0')
		return;

	//check for "#" character, if so, it is a comment
	if (str[0] == '#')
		return;
    
    //check the first word in the command string against the list of existing strings
	//execute the function if exists, else, print command not found
	char cpy[CPROC_LINEBUFF_SIZE + 1];
	strcpy(cpy, str);

	char * cmd = strtok(cpy, self->delimiters);

	int exist = hdlc_CheckCommand(self, cmd);

	//first command exists, invoke it, else exit
	if (exist >= 0) {
		self->cmdList[exist]->cmdFunction(str);
		return;
	}

	//if you got this far, bad command
	self->va_print("command not recognized \n\r\n\r"); 
}

//process events
void hdlc_ProcessEvents(HdlcObj self) {
    
  //store sequence in buf
  //up arrow through putty translates to 0x1b, '[', 'A' sequence
  //down arrow through putty translates to 0x1b, '[', 'B' sequence
  char upSeqTest[] = {'A', '[', 0x1b, 0};     //this is REVERSED, used for testing, don't use for printing
  char downSeqTest[] = {'B', '[', 0x1b, 0};   //this is REVERSED, used for testing, don't use for printing
	char leftSeqTest[] = {'D', '[', 0x1b, 0};   //this is REVERSED, used for testing, don't use for printing
	char rightSeqTest[] = {'C', '[', 0x1b, 0};  //this is REVERSED, used for testing, don't use for printing
  //char leftSeqPrint[] = {0x1b, '[', 'D', 0};  //this is NOT reversed, used for printing
	//char rightSeqPrint[] = {0x1b, '[', 'C', 0}; //this is NOT reversed, used for printing
  char eraseLineSeqPrint[] = {0x1b, '[', '2', 'K', 0}; //this is NOT reversed, used for printing
  int status;
    
    //see if it is a sequence or just a single
    //if there's two char received, might be an up arrow or function key
    char squeue_char[CPROC_CHARBUFF_SIZE + 1];  //get char from the p_recv function
    int seqCount = 0;
    
    //store up to the last 3 characters sequences
    while (self->p_recv(squeue_char) != 0){
        
        //count how many characters are received this loop
        seqCount ++;
        
        //shift everything
        //stores the last sequence of characters here
        self->seqBuf[2] = self->seqBuf[1];
        self->seqBuf[1] = self->seqBuf[0];
        self->seqBuf[0] = squeue_char[0];
    
        //check for sequences of interest, up and down arrows, and escape        
        //check for sequences
        if(self->seqBuf[0] == 0x1b){
            continue;
        }else if(self->seqBuf[1] == 0x1b){
            continue;
        }else if(self->seqBuf[2] == 0x1b){
            
            //up arrow sequence detected
            if(strcmp(upSeqTest, self->seqBuf) == 0){
                
                //store the currently half written command, if any, only when buffer index is -1
                if(self->cmdBufferIndex == -1){
                    cstring_CopyFromArray(self->squeue_msg, self->spare, strlen(self->squeue_msg) + 1);
                }
                
                //increment
                self->cmdBufferIndex++;
                
                //check queue size limit
                if(self->cmdBufferIndex >= (int)self->cmdBuffer->queueSize){
                    self->cmdBufferIndex = self->cmdBuffer->queueSize - 1;
                }
                
                //check objCount limit
                if(self->cmdBufferIndex >= (int)self->cmdBuffer->objCount){
                    self->cmdBufferIndex = self->cmdBuffer->objCount - 1;
                }
                
                //set up a transfer
                status = circbuf_Get(self->cmdBuffer, self->cmdBufferIndex, self->fetch);
                
                //erase line
                self->va_print_src("%s\r", eraseLineSeqPrint);
                
                //load previous command
                if (status == CIRCBUF_TRANS_SUCCESS){
                    strcpy(self->squeue_msg, self->fetch->ptr);
                    self->squeue_msg_index = strlen(self->squeue_msg);
                    self->va_print_src(self->squeue_msg);
                }
                
            }else if(strcmp(downSeqTest, self->seqBuf) == 0){
                
                //increment
                self->cmdBufferIndex--;
                
                //check queue size limit
                if(self->cmdBufferIndex < -1){
                    self->cmdBufferIndex = -1;
                }
                
                //check objCount limit
                if(self->cmdBufferIndex >= (int)self->cmdBuffer->objCount){
                    self->cmdBufferIndex = self->cmdBuffer->objCount - 1;
                }
                
                //set up a transfer, only if we are within the range of the circular buffer
                status = circbuf_Get(self->cmdBuffer, self->cmdBufferIndex, self->fetch);
                
                //erase line
                self->va_print_src("%s\r", eraseLineSeqPrint);
                
                //load previous command
                if (status == CIRCBUF_TRANS_SUCCESS){
                    strcpy(self->squeue_msg, self->fetch->ptr);
                }else if (status == CIRCBUF_TRANS_GETINVALIDOFFSET){
                    strcpy(self->squeue_msg, self->spare->ptr);
                }
                
                self->squeue_msg_index = strlen(self->squeue_msg);
                self->va_print_src(self->squeue_msg);
                
            }else if(strcmp(leftSeqTest, self->seqBuf) == 0){
                //not implemented
                //self->va_print_src("%s", leftSeqPrint);
            }else if(strcmp(rightSeqTest, self->seqBuf) == 0){
                //not implemented
                //self->va_print_src("%s", rightSeqPrint);
            }
            
            //since sequence was found, reset it
            self->seqBuf[2] = 0;
            self->seqBuf[1] = 0;
            self->seqBuf[0] = 0;
            self->escWaitCount = 0;
            continue;
        }
        
        //do not print character if it is an escape character
        self->va_print_src("%c", squeue_char[0]);

		//process character, see if command has been sent
        //process right away if overflow occurs
		if ((squeue_char[0] == '\r') || (self->squeue_msg_index >= CPROC_LINEBUFF_SIZE)){

			//copy command, store in cqueue
			strcpy(self->cqueue_msg, self->squeue_msg);
            
			//reset the index counter
			self->squeue_msg_index = 0;
            self->squeue_msg[self->squeue_msg_index] = '\0';

            //store command into circular buffer
            if((strlen(self->cqueue_msg) > 0)){
				
				if(strcmp(self->cqueue_msg, circbuf_GetPtr(self->cmdBuffer, 0)) != 0){
				
    				//create storage
                    cstring_CopyFromArray(self->cqueue_msg, self->fetch, strlen(self->cqueue_msg) + 1);
                    circbuf_Push(self->cmdBuffer, self->fetch);
				}
            }
            
            //reset the circular buffer index
            self->cmdBufferIndex = -1;
            
			//need to process the command
			hdlc_Invoke(self, self->cqueue_msg);
			return;
		}

		//if backspace press, shift index
		if ((squeue_char[0] == 8 || squeue_char[0] == 127) && self->squeue_msg_index > 0){
			self->squeue_msg_index--;
            self->squeue_msg[self->squeue_msg_index] = '\0';
        }

		//if it reached this far, no need to process command, but store the character, only if it is a character
		if (((squeue_char[0] >= ' ') && (squeue_char[0] <= '~')) || (squeue_char[0] == '\t')){
			self->squeue_msg[self->squeue_msg_index++] = squeue_char[0];
			self->squeue_msg[self->squeue_msg_index] = '\0';
        }
    }
    
    //if the very last character is an escape, wait 2 loops before executing escape
    //escape has been detected
    if(self->seqBuf[0] == 0x1b || self->seqBuf[1] == 0x1b){
            
        //wait for loop limit to reach
        if(self->escWaitCount < CRPOC_ESCAPE_COUNT_LIMIT){
            self->escWaitCount++;
            return;
        }
        
        //if loop limit is reached, consume escape character, reset escape wait
        self->seqBuf[2] = 0;
        self->seqBuf[1] = 0;
        self->seqBuf[0] = 0;
        self->escWaitCount = 0;
        if(self->cmdEscape != '\0'){
			self->cmdEscape->cmdFunction("");
		}
		return;
    }
}
