#include "dev.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

Q_DEFINE_THIS_FILE	

/******************************************************************************
 Device Version Control
******************************************************************************/
#define DEV_VER 0x01    //0.1

/******************************************************************************
define other board specific variables here
******************************************************************************/

//assertion module
AssertObj assertDev;

//systick module
SystickObj systickDev;

//command prompt object
CprocObj procMain;

//define buttons
#define BTN_VOLUP_POS 7
#define BTN_VOLDOWN_POS 6
#define BTN_MODE_POS 0
#define BTN_READ(x, pos) (((x) & (1<<(pos))) >> (pos))

#define BTN_PRESSED_LIMIT 2 //consecutive samples to count as pressed at the systick freq
#define BTN_HOLD_LIMIT 200 //consecutive samples to count as pressed at the systick freq

BtnqpObj btnVolUp;
int ReadBtnVolUp(){return BTN_READ(Btn_Read(), BTN_VOLUP_POS);}

BtnqpObj btnVolDown;
int ReadBtnVolDown(){return BTN_READ(Btn_Read(), BTN_VOLDOWN_POS);}

BtnqpObj btnMode;
int ReadBtnMode(){return BTN_READ(Btn_Read(), BTN_MODE_POS);}


/******************************************************************************
Essential Definitions
******************************************************************************/

//enable/disable interrupts
uint8 state;
void enInt(void){CyGlobalIntEnable;}
void disInt(void){CyGlobalIntDisable;}

//print function wrapper
int p_print(const char * str){
	DbgUart_UartPutString(str);
	return 0;
}

//va_list type print
//define line buffer size
#define LINEBUFFER 255
int va_print(const char *fmt, ...){
    char buffer[LINEBUFFER];
    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(buffer, fmt, args);
    va_end(args);
    DbgUart_UartPutString(buffer);
    return ret;
}

/******************************************************************************
Uart Rx Interrupt definition, called when uart received a character
******************************************************************************/
CY_ISR(DbgUartRx_Int_Handler){
  DbgUartRx_Int_ClearPending();
  DbgUartRx_ClearInterrupt();
  
  QEvent *pe = Q_NEW(QEvent, UART_WAKE_SIG);
  QF_publish(pe);
}

//receive function wrapper
unsigned int p_recv(char *buffer){
	if (DbgUart_SpiUartGetRxBufferSize() != 0){
		*buffer = DbgUart_UartGetChar();
		return 1;
	}else{
		return 0;
	}
}

/******************************************************************************
Timer Interrupt definition, called every 1ms
******************************************************************************/
void SysTick_Handler(void) {                       /* system clock tick ISR */

	//process uart inputs
	cproc_ProcessEvents(procMain);
    
  //process buttons
  btnqp_Proc(btnVolUp);
  btnqp_Proc(btnVolDown);
  btnqp_Proc(btnMode);
  
  //service the FSM
  QF_TICK_X(0U, (void *)0);              /* process all armed time events */
}

/******************************************************************************
Timer Interrupt for checking battery and lights
******************************************************************************/
CY_ISR(PeriodicCheck_Int_Handler){
  PeriodicCheck_Int_ClearPending();
  CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
  
  QEvent *pe = Q_NEW(QEvent, PERIODIC_WAKE_SIG);
  QF_publish(pe);
}

/******************************************************************************
Interrupt for Timer
******************************************************************************/
CY_ISR(SpeakerPause_Int_Handler){  
  SpeakerPause_Int_ClearPending();
  TimerOnOff_ClearInterrupt(TimerOnOff_INTR_MASK_TC);
  
  QEvent *pe = Q_NEW(QEvent, SpeakerPause_SIG);
  QF_publish(pe);
}


/******************************************************************************
Button interrupt for user input
******************************************************************************/
CY_ISR(Btn_Int_Handler){
  Btn_Int_ClearPending();
  Btn_ClearInterrupt();

  QEvent *pe = Q_NEW(QEvent, BUTTON_WAKE_SIG);
  QF_publish(pe);
}

/******************************************************************************
BSP Specific Functions
******************************************************************************/
//set up analog
#define SAR_CHANNELS 1
#define AMUX_CHANNELS 2

//battery detection thresholds
#define BATTERY_THRESHOLD_FALLING 960
#define BATTERY_THRESHOLD_RISING  980

//lowlight detection thresholds
#define LOWLIGHT_THRESHOLD_FALLING 90
#define LOWLIGHT_THRESHOLD_RISING  120

#define LOWLIGHT_TRACKER_HIGH  2
#define LOWLIGHT_TRACKER_LOW  -2
int lowlight_tracker = 0;

//check battery, check day/night
void BSP_processPeriodic(int * led_powerSuspend, int * led_speakerOn, int * led_lowLight, int * led_lowBattery){
  
  //display the status of the previous BSP_checkAnalog call
  if (*led_powerSuspend == LED_ON)  LED_DEVICEON_Write(1);
  if (*led_speakerOn == LED_ON)     LED_SPKRON_Write(1);
  if (*led_lowLight == LED_ON)      LED_LOWLIGHT_Write(1);
  if (*led_lowBattery == LED_ON)    LED_LOWBATT_Write(1);
  
  //ADC takes longest to wake up
  SAR_Wakeup(); 
  CyDelayUs(1900);  //tested and optimized at 3.0V, use 1.9mS (can be faster at lower voltages it seems)
  
  //shut down all LEDs
  LED_DEVICEON_Write(0);
  LED_SPKRON_Write(0);
  LED_LOWLIGHT_Write(0);
  LED_LOWBATT_Write(0);
  
  //turn on detection
  //keep them on all the time, total draw is less than 10uA
  //LightDetectEn_Write(0);
  //BattDetectEn_Write(0);
  
  //start analog blocks
  DetectMux_Start();
  DetectBuf_Wakeup();
  CyDelayUs(10);
  
  //acquire the analog signals
  int i;
  int data[AMUX_CHANNELS];
  for(i = 0; i < AMUX_CHANNELS; i ++){
    DetectMux_Select(i);
    CyDelayUs(25);
    
    SAR_StartConvert();
    SAR_IsEndConversion(SAR_WAIT_FOR_RESULT);
    data[i] = SAR_CountsTo_mVolts(SAR_CHANNELS - 1, SAR_GetResult16(SAR_CHANNELS - 1));
  }
  
  //shut down analog blocks
  SAR_Sleep();
  DetectBuf_Sleep();
  DetectMux_Stop();
  
  //turn off detection
  //LightDetectEn_Write(1); //keep dete
  //BattDetectEn_Write(1);
  
  //update led flags for next call
  if(data[0] < BATTERY_THRESHOLD_FALLING){
    *led_lowBattery = LED_ON;
  }else if(data[0] > BATTERY_THRESHOLD_RISING){
    *led_lowBattery = LED_OFF;
  }

  //check if low light has been trending for the last few calls
  //if a threshold change has occured, shutdown or wakeup
  if(data[1] < LOWLIGHT_THRESHOLD_FALLING){
    *led_lowLight = LED_ON;
    lowlight_tracker--;
  }else if(data[1] > LOWLIGHT_THRESHOLD_RISING){
    *led_lowLight = LED_OFF;
    lowlight_tracker++;
  }
  
  //high light for long time, wake up
  if(lowlight_tracker > LOWLIGHT_TRACKER_HIGH){
    lowlight_tracker = LOWLIGHT_TRACKER_HIGH;
    
    QEvent *pe = Q_NEW(QEvent, LOWLIGHT_WAKE_SIG);
    QF_publish(pe);
    
  //low light for long time, go to sleep
  }else if(lowlight_tracker < LOWLIGHT_TRACKER_LOW){
    lowlight_tracker = LOWLIGHT_TRACKER_LOW;
    
    QEvent *pe = Q_NEW(QEvent, LOWLIGHT_SLEEP_SIG);
    QF_publish(pe);
  }
   
  //display status, store for next call
  va_print("Batt %4d, Light %4d, LightTracker %2d\n\r", data[0], data[1], lowlight_tracker);
}

//welcome message waking up
void BSP_wake(void){
  
  //turn on sine gen components
  SineGenBuf_Start();
  SquareGen_Start();
  SquareTimer_Start();
  TimerOnOff_Start();
  CyDelay(10u);
  
  //turn on boost and speaker driver
  BOOST_SHDN_B_Write(1);
  CyDelay(10u);
  
  SPKR_SHDN_B_Write(1);
  CyDelay(10u);
  
  cproc_Flush(procMain);  //flush out garbage character, reset command prompt
  CyDelay(10u);
  
}

//go to deep sleep
void BSP_sleep(void){

  //turn on sine gen components
  SineGenBuf_Stop();
  SquareGen_Stop();
  SquareTimer_Stop();
  TimerOnOff_Stop();
  
  //turn off boost and speaker driver
  BOOST_SHDN_B_Write(0);
  SPKR_SHDN_B_Write(0);
  
  CyDelay(10u);

  CySysPmDeepSleep();
}

//welcome message waking up
void BSP_pause_wake(void){
  
  //turn on sine gen components
  SineGenBuf_Start();
  SquareGen_Start();
  SquareTimer_Start();
  CyDelay(10u);
  
  //turn on boost and speaker driver
  BOOST_SHDN_B_Write(1);
  CyDelay(10u);
  
  SPKR_SHDN_B_Write(1);
  CyDelay(10u);
  
}

//go to pause 30s
void BSP_pause(void){

  //turn on sine gen components
  SineGenBuf_Stop();
  SquareGen_Stop();
  SquareTimer_Stop();
  
  //turn off boost and speaker driver
  BOOST_SHDN_B_Write(0);
  SPKR_SHDN_B_Write(0); 
}

//freq look up
int freqLut[FREQ_MAX - FREQ_MIN + 1] = {
  450, 500, 550, 600
};

int freqCalcPeriod(int freq){
  return(BSP_PRIMARY_OSC_HZ/(2*freq) - 1);
}

//process frequency
void BSP_setFreq(int freqIndex){
  SquareTimer_WritePeriod(freqCalcPeriod(freqLut[freqIndex]));
  va_print("frequency set to %d Hz\r\n", freqLut[freqIndex]);
}

//freq up
void BSP_freqUp(int * freqIndex){
  if (*freqIndex < FREQ_MAX){
    (*freqIndex)++;
  }
  BSP_setFreq(*freqIndex);
}

//freq down
void BSP_freqDown(int * freqIndex){
  if (*freqIndex > FREQ_MIN){
    (*freqIndex)--;
  }
  BSP_setFreq(*freqIndex);
}

//volume look up
int volumeLut[VOLUME_MAX - VOLUME_MIN + 1] = {
  1, 2, 3, 4, 5
};

//process volume
void BSP_setVolume(int volume){
  int high = (SquareGen_ReadPeriod() + 1)/2 + volumeLut[volume];
  int low = (SquareGen_ReadPeriod() + 1)/2 - volumeLut[volume];
  SquareGen_WriteCompare(high);
  SquareGen_WriteCompareBuf(low);
  va_print("Volume %d, Hi %d, Lo %d\r\n", volume, high, low);
}

//volume up
void BSP_volUp(int * volume){
  if (*volume < VOLUME_MAX){
    (*volume)++;
  }
  BSP_setVolume(*volume);
}

//volume down
void BSP_volDown(int * volume){
  if (*volume > VOLUME_MIN){
    (*volume)--;
  }
  BSP_setVolume(*volume);
}

//function called from cold start
void BSP_init(void) {
	int status;

	//init UART, clear UART screen
	DbgUart_Start();
	DbgUart_UartPutChar(0x0c);
	va_print("\f\r\n");
  va_print("Debug Sound Lure v1 ver 0.2\r\n");
	
	//initialize assertion debug module
	assertDev = assert_New();
	assert_IsZero(assert_Init(assertDev, va_print));

	//set up the command processing module
	procMain = cproc_New();
	status = cproc_Init(procMain, enInt, disInt, va_print, p_recv, " \t", 1);	//delimite using " " and "\t"
	assert_IsZeroPrint(assertDev, status, "Cproc Init");
	
  status = cproc_AddEscape(procMain, &ucmdEscape);     //callback when escape character is detected
	status = cproc_Add(procMain, "stat", &ucmdStatus);	//list the values in LUT
	status = cproc_Add(procMain, "stop", &ucmdStop);	    //list the values in LUT
  status = cproc_Add(procMain, "ltest", &ucmdLedTest);	//test the LEDs
  status = cproc_Add(procMain, "sleep", &ucmdSleep);    //go to deepsleep
  assert_IsZeroPrint(assertDev, status, "Cproc Add");
  
  //turn off light detect, battery detect
  BattDetectEn_Write(0);
  LightDetectEn_Write(0);
  DetectBuf_Start();
  SAR_Start();  
  
  //set speaker driver gain to 0
  SPKR_GAIN_Write(0);
  
  DbgUartRx_Int_StartEx(DbgUartRx_Int_Handler);
  PeriodicCheck_Int_StartEx(PeriodicCheck_Int_Handler);
  Btn_Int_StartEx(Btn_Int_Handler);
  SpeakerPause_Int_StartEx(SpeakerPause_Int_Handler);
  
  //start button debouncers
  btnVolUp = btnqp_New();
  btnqp_Init(btnVolUp, ReadBtnVolUp, BTNVOLUP_FALL_SIG, BTNVOLUP_HOLD_SIG, BTNVOLUP_RISE_SIG, BTN_PRESSED_LIMIT, BTN_HOLD_LIMIT);
  
  btnVolDown = btnqp_New();
  btnqp_Init(btnVolDown, ReadBtnVolDown, BTNVOLDOWN_FALL_SIG, BTNVOLDOWN_HOLD_SIG, BTNVOLDOWN_RISE_SIG, BTN_PRESSED_LIMIT, BTN_HOLD_LIMIT);
  
  btnMode = btnqp_New();
  btnqp_Init(btnMode, ReadBtnMode, BTNMODE_FALL_SIG, BTNMODE_HOLD_SIG, BTNMODE_RISE_SIG, BTN_PRESSED_LIMIT, BTN_HOLD_LIMIT);
  
  //turn on detection
  //keep them on all the time, total draw is less than 10uA
  LightDetectEn_Write(0);
  BattDetectEn_Write(0);
  
  va_print("Init Complete\n\r\n\r");
  
	CyGlobalIntEnable;
}

/******************************************************************************
QP functions
******************************************************************************/
/*..........................................................................*/
void QF_onStartup(void) {
	
	//link ISR vector
	CyIntSetSysVector(SYSTICK_INTERRUPT_VECTOR_NUMBER, SysTick_Handler);
  
  //system tick, use constants in bsp.h
  SysTick_Config(BSP_PRIMARY_OSC_HZ / BSP_TICKS_PER_SEC);

  //set priority for systick interupt
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIO);
	
	//enable interrupts
	CyGlobalIntEnable;
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QF_onIdle(void) {       /* called with interrupts disabled, see NOTE01 */

	CySysPmSleep();
  CyGlobalIntEnable;
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_DISABLE();         /* make sure that all interrupts are disabled */
    for (;;) {       /* NOTE: replace the loop with reset for final version */
    }
}
/*..........................................................................*/
/* error routine that is called if the CMSIS library encounters an error    */
void assert_failed(char const *file, int line) {
    Q_onAssert(file, line);
}