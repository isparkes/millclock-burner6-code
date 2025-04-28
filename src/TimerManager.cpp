#include "TimerManager.h"

hw_timer_t * timer0 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;

hw_timer_t * timer1 = NULL;
extern portMUX_TYPE timerMux1;

hw_timer_t * timer2 = NULL;
portMUX_TYPE timerMux2 = portMUX_INITIALIZER_UNLOCKED;

volatile int count0;
volatile int count0Max = COUNT0_MAX;
volatile int count0Off = COUNT0_OFF;

// These variables hold the impression data
extern volatile uint32_t val1;

volatile uint32_t _dispVal = 0;
volatile uint8_t _dispBoardCount = 3;

// ************************************************************
// ISR for LED flash update
// ************************************************************
void IRAM_ATTR onTimer0() {
   portENTER_CRITICAL_ISR(&timerMux0);
   count0++;
   if (count0 > count0Max) {
     count0 = 0;
     digitalWrite(LED_PIN, HIGH);
   } else if (count0 == count0Off) {
     digitalWrite(LED_PIN, LOW);
   }
   portEXIT_CRITICAL_ISR(&timerMux0);
}

// ************************************************************
// Perform the parallel shift out to the registers. Always
// output both digits of a tube board.
// ************************************************************
void IRAM_ATTR shiftOutDigits() {
  uint8_t i;
  uint8_t j;

  // output dispcount copies of dispval
  for(j = 0; j < _dispBoardCount; j++) {
    for (i = 0; i < 24; i++) {
      digitalWrite(DATAPin, !!(_dispVal & (1 << (23 - i))));
      digitalWrite(CLKPin, HIGH);
      digitalWrite(CLKPin, LOW);
    }
    // Latch it in
    digitalWrite(LATCHPin, HIGH);
    digitalWrite(LATCHPin, LOW);
  }
}

// ************************************************************
// ISR for display update.
// This is called every 500uS. It will load the new values
// from the outputManager and shift them out to the display
// registers. The display is updated every 500uS, but the
// display is only updated if the value has changed.
//
// We output 3 8-bit shift registers for each digit 
// pair = 24 bits, but only 22 bits are used.
//
// We also have an interlock between writing the data into the
// registers (dispVal, dispBoardCount) using the
// timerMux1 mutex. If we don't have this, the display will
// glitch due to the values changing while displaying).
//
// This is double-buffering: The values are calculated into
// temporary variables in OutputManager_::outputDisplay, then
// loaded into the local volatile variables ONCE PER IMPRESSION.
// ************************************************************
void IRAM_ATTR onTimer1() {
  portENTER_CRITICAL_ISR(&timerMux1);

  // Load the new values from the output of the outputManager
  _dispVal = dispVal;
  _dispBoardCount = dispBoardCount;

  shiftOutDigits();

  portEXIT_CRITICAL_ISR(&timerMux1);
}

// ************************************************************
// Start the timers
// ************************************************************
void startTimers() {
  // LED flash timer
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &onTimer0, true);
  timerAlarmWrite(timer0, 10000, true);
  // https://community.platformio.org/t/hardware-timer-issue-with-esp32/22047/10
  delayMicroseconds(0);
  timerAlarmEnable(timer0);

  // Display time
  timer1 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 500, true);
  // https://community.platformio.org/t/hardware-timer-issue-with-esp32/22047/10
  delayMicroseconds(0);
  timerAlarmEnable(timer1);

  // Set default LED flash type
  setLedFlashType(1);
}

// ************************************************************
// Set the LED flash type
// 0: Connected - short flash 1/s
// 1: Connecting - long flash 2/3s
// ************************************************************
void setLedFlashType(byte flashType) {
  switch(flashType) {
    case 0: {
      count0Max = 100;
      count0Off = 1;
      break;
    }
    case 1: {
      count0Max = 100;
      count0Off = 50;
      break;
    }
  }
}

