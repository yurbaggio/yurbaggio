//includes
#include "dev.h"

//define the file
Q_DEFINE_THIS_FILE

//create active object class
typedef struct MainFsmTag {
	QActive super;
	QTimeEvt postEveryEvt;	//1 second time event generator
	QTimeEvt postInEvt;		//configurable delay event generator
  
  //volume setting
  int volume;
  
  //frequency setting
  int freqIndex;
  
  //led settings
  int led_powerSuspend;
  int led_speakerOn;
  int led_lowLight;
  int led_lowBattery;
} MainFsm;

//state table
static QState MainFsm_Initial(MainFsm *me, QEvent const *e);
static QState MainFsm_Boot(MainFsm *me, QEvent const *e);
static QState MainFsm_Top(MainFsm *me, QEvent const *e);
static QState MainFsm_SpeakerOn(MainFsm *me, QEvent const *e);
static QState MainFsm_SpeakerOff(MainFsm *me, QEvent const *e);
static QState MainFsm_SpeakerOffUart(MainFsm *me, QEvent const *e);
static QState MainFsm_SpeakerOffLowlight(MainFsm *me, QEvent const *e);
static QState MainFsm_SpeakerPause(MainFsm *me, QEvent const *e);

//single instance of the FSM object
static MainFsm mainFsm_inst;

//create the pointer to the active object
QActive * const AO_MainFsm = (QActive *)&mainFsm_inst;	//AO pointer

//constructor
void MainFsm_ctor(void) {
	MainFsm *me = &mainFsm_inst;	//point state machines to the main object
  me->volume = VOLUME_DEF;
  me->freqIndex = FREQ_DEF;
  
	QActive_ctor(&me->super, (QStateHandler)&MainFsm_Initial);	//instance super, set initial state
	QTimeEvt_ctor(&me->postEveryEvt, POSTEVERY_SIG);
	QTimeEvt_ctor(&me->postInEvt, POSTIN_SIG);
}

//initializer
QState MainFsm_Initial(MainFsm *me, QEvent const *e){
	(void)e;
  
	//QActive_subscribe((QActive *)me, BTNVOLUP_FALL_SIG);
  QActive_subscribe((QActive *)me, BTNVOLUP_RISE_SIG);
  QActive_subscribe((QActive *)me, BTNVOLUP_HOLD_SIG);
  //QActive_subscribe((QActive *)me, BTNVOLDOWN_FALL_SIG);
  QActive_subscribe((QActive *)me, BTNVOLDOWN_RISE_SIG);
  QActive_subscribe((QActive *)me, BTNVOLDOWN_HOLD_SIG);
  //QActive_subscribe((QActive *)me, BTNMODE_FALL_SIG);
  QActive_subscribe((QActive *)me, BTNMODE_RISE_SIG);
  QActive_subscribe((QActive *)me, BTNMODE_HOLD_SIG);
  QActive_subscribe((QActive *)me, UART_WAKE_SIG);
  QActive_subscribe((QActive *)me, UART_SLEEP_SIG);
  QActive_subscribe((QActive *)me, LOWLIGHT_WAKE_SIG);
  QActive_subscribe((QActive *)me, LOWLIGHT_SLEEP_SIG);  
  QActive_subscribe((QActive *)me, PERIODIC_WAKE_SIG);
  QActive_subscribe((QActive *)me, BUTTON_WAKE_SIG);
  QActive_subscribe((QActive *)me, BUTTON_SLEEP_SIG);
  QActive_subscribe((QActive *)me, SpeakerPause_SIG);
  //QActive_subscribe((QActive *)me, CMD_WAKE_SIG);
  //QActive_subscribe((QActive *)me, CMD_SLEEP_SIG);
	return Q_TRAN(&MainFsm_Boot);	//initial transition from init pseudostate
}

//wait for all systems to power on, then go into state machine
QState MainFsm_Boot(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
			QTimeEvt_postIn(&me->postInEvt, (QActive *)me, START_DELAY);		//start system boot delay
			return Q_HANDLED();

		case POSTIN_SIG:
			return Q_TRAN(&MainFsm_Top);
			
		case Q_EXIT_SIG:
			QTimeEvt_disarm(&me->postInEvt);    //reset the timer event
			return Q_HANDLED();
	}
	return Q_SUPER(&QHsm_top);
}

QState MainFsm_Top(MainFsm *me, QEvent const *e){
	switch (e->sig) {
    
    //wake up speakers, turn speaker on initially
		case Q_INIT_SIG:
      BSP_wake();
      BSP_setFreq(me->freqIndex);
      BSP_setVolume(me->volume);
			return Q_TRAN(&MainFsm_SpeakerOn);
      
		case Q_ENTRY_SIG:
			return Q_HANDLED();
      
		case Q_EXIT_SIG:
			return Q_HANDLED();
	}
	return Q_SUPER(&QHsm_top);
}

QState MainFsm_SpeakerOn(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
      me->led_speakerOn = LED_ON;
			return Q_HANDLED();  
      
    //freq up
    case BTNVOLDOWN_HOLD_SIG:
      BSP_freqDown(&(me->freqIndex));
      return Q_HANDLED();
      
    //freq down
    case BTNVOLUP_HOLD_SIG:
      BSP_freqUp(&(me->freqIndex));
      return Q_HANDLED();
      
    //volume up    
    case BTNVOLUP_RISE_SIG:
      BSP_volUp(&(me->volume));
      return Q_HANDLED();
    
    //volume down      
    case BTNVOLDOWN_RISE_SIG:
      BSP_volDown(&(me->volume));
      return Q_HANDLED();
    
    //transition to speaker pause state
    case SpeakerPause_SIG:
      return Q_TRAN(&MainFsm_SpeakerPause);
    
    //transition to speaker off state
    case BTNMODE_HOLD_SIG:
      return Q_TRAN(&MainFsm_SpeakerOff);
    case UART_SLEEP_SIG:
      return Q_TRAN(&MainFsm_SpeakerOffUart);
		case LOWLIGHT_SLEEP_SIG:
      return Q_TRAN(&MainFsm_SpeakerOffLowlight);
      
    case PERIODIC_WAKE_SIG:
      BSP_processPeriodic(&(me->led_powerSuspend), &(me->led_speakerOn), &(me->led_lowLight), &(me->led_lowBattery));
      return Q_HANDLED();      
      
		case Q_EXIT_SIG:
      me->led_speakerOn = LED_OFF;
			return Q_HANDLED();
	}
	return Q_SUPER(&MainFsm_Top);
}

QState MainFsm_SpeakerPause(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
      BSP_pause();
      me->led_speakerOn = LED_ON;
			return Q_HANDLED();  
      
    //freq up
    case BTNVOLDOWN_HOLD_SIG:
      BSP_freqDown(&(me->freqIndex));
      return Q_HANDLED();
      
    //freq down
    case BTNVOLUP_HOLD_SIG:
      BSP_freqUp(&(me->freqIndex));
      return Q_HANDLED();
      
    //volume up    
    case BTNVOLUP_RISE_SIG:
      BSP_volUp(&(me->volume));
      return Q_HANDLED();
    
    //volume down      
    case BTNVOLDOWN_RISE_SIG:
      BSP_volDown(&(me->volume));
      return Q_HANDLED();
    
    //transition to speaker pause state
    case SpeakerPause_SIG:
      BSP_pause_wake();
      return Q_TRAN(&MainFsm_SpeakerOn);
    
    //transition to speaker off state
    case BTNMODE_HOLD_SIG:
      return Q_TRAN(&MainFsm_SpeakerOff);
    case UART_SLEEP_SIG:
      return Q_TRAN(&MainFsm_SpeakerOffUart);
		case LOWLIGHT_SLEEP_SIG:
      return Q_TRAN(&MainFsm_SpeakerOffLowlight);
      
    case PERIODIC_WAKE_SIG:
      BSP_processPeriodic(&(me->led_powerSuspend), &(me->led_speakerOn), &(me->led_lowLight), &(me->led_lowBattery));
      return Q_HANDLED();      
      
		case Q_EXIT_SIG:      
      me->led_speakerOn = LED_OFF;
			return Q_HANDLED();
	}
	return Q_SUPER(&MainFsm_Top);
}

QState MainFsm_SpeakerOff(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
      BSP_sleep();
      va_print("Entering deep sleep\r\n");
      me->led_powerSuspend = LED_ON;
			return Q_HANDLED();

    //run analog checks, then go back to sleep
    case PERIODIC_WAKE_SIG:
      BSP_processPeriodic(&(me->led_powerSuspend), &(me->led_speakerOn), &(me->led_lowLight), &(me->led_lowBattery));
      BSP_sleep();
      return Q_HANDLED();
    
    case BUTTON_WAKE_SIG:
      return Q_TRAN(&MainFsm_SpeakerOn);
    
    //if we get a UART WAKE or LOWLIGHT WAKE ignore
    case UART_WAKE_SIG:
    case LOWLIGHT_WAKE_SIG:
      return Q_HANDLED();
      
		case Q_EXIT_SIG:
      BSP_wake();
      va_print("Waking from deep sleep\r\n"); 
      me->led_powerSuspend = LED_OFF;
			return Q_HANDLED();
	}
	return Q_SUPER(&MainFsm_Top);
}

QState MainFsm_SpeakerOffUart(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
			return Q_HANDLED();
    
    case UART_WAKE_SIG:
      return Q_TRAN(&MainFsm_SpeakerOn);
      
		case Q_EXIT_SIG:
			return Q_HANDLED();
	}
	return Q_SUPER(&MainFsm_SpeakerOff);
}

QState MainFsm_SpeakerOffLowlight(MainFsm *me, QEvent const *e){
	switch (e->sig) {
		case Q_ENTRY_SIG:
			return Q_HANDLED();
    
    case LOWLIGHT_WAKE_SIG:
      return Q_TRAN(&MainFsm_SpeakerOn);
      
		case Q_EXIT_SIG:
			return Q_HANDLED();
	}
	return Q_SUPER(&MainFsm_SpeakerOff);
}