#ifndef NLIB_SYS_QUEUE_TEST_H
#define NLIB_SYS_QUEUE_TEST_H

#include "nlib_sys_queue.h"

//print what's in the list
void nq_TestPrint(NqObj queue, int (*va_print)(const char *format, ...));

//run test
void nq_Test(void (*enInt)(void), void (*disInt)(void), int (*va_print)(const char *format, ...));

#endif

/* [] END OF FILE */
