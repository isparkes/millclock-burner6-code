#include "OutputManager.h"

// ************************************************************
// Break the time into displayable digits
// ************************************************************
void OutputManager_::loadNumberArrayIntegerValue(unsigned int value) {
  unsigned int valueBound = value;
  if (valueBound > 999999)
    valueBound = 999999;
  
  byte s1 = valueBound % 10;
  valueBound = valueBound / 10;
  byte s10 = valueBound % 10;
  valueBound = valueBound / 10;
  byte m1 = valueBound % 10;
  valueBound = valueBound / 10;
  byte m10 = valueBound % 10;
  valueBound = valueBound / 10;
  byte h1 = valueBound % 10;
  valueBound = valueBound / 10;
  byte h10 = valueBound % 10;

  numberArray[H1]  = convertToDigit(h1  % 10);
  numberArray[H10] = convertToDigit(h10 % 10);

  // Update buffers
  outputDisplay();
}

// ************************************************************
// Break the time into displayable digits
// ************************************************************
void OutputManager_::loadNumberArraySameValue(byte value) {
  byte val = value % 10;
  numberArray[H1]  = convertToDigit(val);
  numberArray[H10] = convertToDigit(val);

  // Update buffers
  outputDisplay();
}

// ************************************************************
// Do a single complete display, including any fading and
// dimming requested. Prepares the display variables for
// the interrupt driven display output.
// This is the heart of the display processing!
// ************************************************************
void OutputManager_::outputDisplay() {
  uint32_t tmpnextVal1 = decodeFromNumberArray(
                                numberArray[H10], 
                                numberArray[H1],
                                blanked,
                                blanked,
                                false,      // separators not used
                                false);     // separators not used

  
  // move the values over, respect the MUTEX on the interrupt, otherwise we get visible glitches
  portENTER_CRITICAL_ISR(&timerMux1);
  val1 = tmpnextVal1;
  portEXIT_CRITICAL_ISR(&timerMux1);
}

// ************************************************************
// Turn a display pair into a uint24 ready for output
// ************************************************************
uint32_t OutputManager_::decodeFromNumberArray(byte valueToDecodeTens, byte valueToDecodeUnits, bool blankTens, bool blankUnits, bool led1, bool led2) {
  uint32_t decoded = 0;
  if (!blankTens) decoded = DECODE_DIGIT[valueToDecodeTens];
  if (!blankUnits) decoded = decoded | DECODE_DIGIT[valueToDecodeUnits] << 10;
  if (led1) decoded |= DECODE_LED[0];
  if (led2) decoded |= DECODE_LED[1];
  return decoded;
}

// ************************************************************
// Set the mode we are in
// ************************************************************
void OutputManager_::updateOncePerSecond() {
#ifdef OTM_EXTENDED_DEBUG
dumpNumberArrayValues();
#endif

}

// ************************************************************
// Safety function: Convert given value to a valid digit value
// ************************************************************
clock_digit OutputManager_::convertToDigit(int value) {
  if (value < 0) {
    debugMsgOtm("Underrange error converting digit");
    debugMsgOtm("Got: " + String(value));
    return digit0;
  }
  if (value > 9) {
    debugMsgOtm("Overrange error converting digit");
    debugMsgOtm("Got: " + String(value));
    return digit9;
  }
  return (clock_digit) value;
}

#ifdef OTM_EXTENDED_DEBUG
void OutputManager_::dumpNumberArrayValues() {
  String val = "Number Array: " + 
    String(numberArray[0]) + String(numberArray[1]) + ":" + 
    String(numberArray[2]) + String(numberArray[3]) + ":" +
    String(numberArray[4]) + String(numberArray[5]);
  debugMsgOtm(val);

}
#endif

// ************************************************************
// Library internal singleton wiring
// ************************************************************
OutputManager_ &OutputManager_::getInstance() {
  static OutputManager_ instance;
  return instance;
}

OutputManager_ &outputManager = outputManager.getInstance();