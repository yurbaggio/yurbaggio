#ifndef NLIB_CSTRING_QUEUE_TEST_H_
#define NLIB_CSTRING_QUEUE_TEST_H_

//test
#define CQUEUE_TESTSTRINGSIZE 10
#define CQUEUE_TESTQUEUESIZE 5

//initialize test
int cqueue_TestQueueInit(void (*enInt_var)(void), void (*disInt_var)(void), int (*va_print_var)(const char *, ...));

//debug enqueue, dequeue
void cqueue_TestEnq(char *str);
void cqueue_TestDeq(char *str);

#endif
