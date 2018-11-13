/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef BSP_H
#define BSP_H

//board support
#include "dev.h"

/******************************************************************************
User Define
******************************************************************************/
#define LED_ON  1
#define LED_OFF 0

#define FREQ_MAX 3
#define FREQ_MIN 0
#define FREQ_DEF 1
  
#define VOLUME_MAX 4
#define VOLUME_MIN 0
#define VOLUME_DEF 0
  
/******************************************************************************
Extern accessible variables and functions
******************************************************************************/

//main trap object
//extern ToneGenObj toneDev;
  
//systick module
extern SystickObj systickDev;

//EEPROM Settings
typedef struct eepromData{
    
    //first boot, load default if first boot
    unsigned char fboot;
}EepromData;

extern EepromData eeDev;

//va_list type print
int va_print(const char *fmt, ...);

/******************************************************************************
Interrupt Priority
******************************************************************************/
#define SYSTICK_INTERRUPT_VECTOR_NUMBER 15u /* Cortex-M0 hard vector */
	
enum KernelUnawareISRs {                                      /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI                        /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI,                  /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI                          /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/******************************************************************************
Timing control bits
******************************************************************************/
/* prmiary oscillator frequency */
#define BSP_PRIMARY_OSC_HZ      4000000U

/* the system tick rate [Hz] */   
#define BSP_TICKS_PER_SEC    	100UL	//10 ms ticks

/*CPU frequency (FOSC / 2) */
//#define BSP_FCY_HZ              (BSP_PRIMARY_OSC_HZ / 2)

//define time constant that converts ticks to minutes
//#define K_TICK1MIN 60000	//since 1tick = 1ms, 60000 ticks = 1 min

//define delay variables
#define START_DELAY	100		//x10ms
	
/******************************************************************************
State Machine definitions
******************************************************************************/
#define SMALLPOOLSIZE 25U

enum MainFsmSignals {
	DUMMY_SIG = Q_USER_SIG,     /* published by Table to let a philosopher eat */
  BTNVOLUP_FALL_SIG,
  BTNVOLUP_RISE_SIG,
  BTNVOLUP_HOLD_SIG,
  BTNVOLDOWN_FALL_SIG,
  BTNVOLDOWN_RISE_SIG,
  BTNVOLDOWN_HOLD_SIG,
  BTNMODE_FALL_SIG,
  BTNMODE_RISE_SIG,
  BTNMODE_HOLD_SIG,
  UART_WAKE_SIG,
  UART_SLEEP_SIG,
  LOWLIGHT_WAKE_SIG,
  LOWLIGHT_SLEEP_SIG,
  PERIODIC_WAKE_SIG,
  SpeakerPause_SIG,
  BUTTON_WAKE_SIG,
  BUTTON_SLEEP_SIG,
  //CMD_WAKE_SIG,
  //CMD_SLEEP_SIG,
  MAX_PUB_SIG,                                /* the last published signal */
	POSTEVERY_SIG,
	POSTIN_SIG,
	MAX_SIG                                               /* the last signal */
};	

void MainFsm_ctor(void);
extern QActive * const AO_MainFsm;

/******************************************************************************
BSP Function Prototypes
******************************************************************************/
//board specific functions
void BSP_init(void);

//welcome message waking up
void BSP_wake(void);

//go to deep sleep
void BSP_sleep(void);

//go to pause and wake
void BSP_pause(void);
void BSP_pause_wake(void);

//process frequency
void BSP_setFreq(int freq);

//freq up
void BSP_freqUp(int * freq);

//freq down
void BSP_freqDown(int * freq);

//process volume
void BSP_setVolume(int volume);

//volume up
void BSP_volUp(int * volume);

//volume down
void BSP_volDown(int * volume);

//check battery, check day/night
void BSP_processPeriodic(int * led_powerSuspend, int * led_speakerOn, int * led_lowLight, int * led_lowBattery);

//process volume
//void BSP_refreshVolume(void);

#endif
/* [] END OF FILE */
