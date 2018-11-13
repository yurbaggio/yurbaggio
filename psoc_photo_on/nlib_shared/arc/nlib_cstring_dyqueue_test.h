/*
 * nlib_per_sproc_Dyqueue.h
 *
 *	Xilinx AXI_QUAD_SPI in standard mode, transfer queue
 *
 *  Created on: Nov 23, 2015
 *      Author: Jianyi
 */

#ifndef NLIB_PER_CSTRING_DYQUEUE_TEST_H
#define NLIB_PER_CSTRING_DYQUEUE_TEST_H

#include <stdarg.h>

//test
#define DYQUEUE_TESTQUEUESIZE 5

//initialize test
int dyqueue_TestQueueInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print)(const char *, ...));

//debug enqueue, dequeue
void dyqueue_TestEnq(const char *str);
void dyqueue_TestDeq(const char *str);

#endif
