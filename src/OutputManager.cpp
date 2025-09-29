#include "OutputManager.h"

// ************************************************************
// Load the digit to display into the internal buffer
// ************************************************************
void OutputManager_::loadNumberIntegerValue(unsigned int newValue) {
  if (cc->tubeType == ZIN70) {
    decodedValue = decodeFromNumberArray(newValue  % CATHODE_COUNT_ZIN70);
  } else if (cc->tubeType == ZIN18) {
    decodedValue = decodeFromNumberArray(newValue  % CATHODE_COUNT_ZIN18);
  } else {
    debugManager.debugMsg("OutputManager", "Unknown tube type");
  }

  #ifdef OTM_EXTENDED_DEBUG
  debugMsgOtm("Received value: " + String(newValue));
  #endif

  // Update buffers
  outputDisplay();
}

// ************************************************************
// Do a single complete display. Prepares the display variables for
// the interrupt driven display output.
// ************************************************************
void OutputManager_::outputDisplay() {
  // move the values over, respect the MUTEX on the interrupt, otherwise we get visible glitches
  portENTER_CRITICAL_ISR(&timerMux1);
  dispVal = decodedValue;
  dispBoardCount = cc->tubeBoardCount;
  portEXIT_CRITICAL_ISR(&timerMux1);
}

// ************************************************************
// Turn a display pair into a uint32 ready for output.
// Ayways prepare the entire width of TWO digits, even though we
// might only use 1 digit.
// ************************************************************
uint32_t OutputManager_::decodeFromNumberArray(byte valueToDecode) {
  uint32_t decodedTwoDigits = 0;
  uint32_t decodedOneDigit = 0;

  // for the case that our 595s don't have and output enable pin
  if (!blanked) {
    if (cc->tubeType == ZIN70) {
      decodedOneDigit = DECODE_DIGIT_ZIN70[valueToDecode];    
    } else if (cc->tubeType == ZIN18) {
      decodedOneDigit = DECODE_DIGIT_ZIN18[valueToDecode];
    }
  }

  #ifdef OTM_EXTENDED_DEBUG
  debugMsgOtm("Decoded one value: " + String(decodedOneDigit));
  #endif

  decodedTwoDigits = decodedOneDigit << 11 | decodedOneDigit;

  #ifdef OTM_EXTENDED_DEBUG
  debugMsgOtm("Decoded two values: " + String(decodedTwoDigits));
  #endif

  return decodedTwoDigits;
}

// ************************************************************
// Set the mode we are in
// ************************************************************
void OutputManager_::updateOncePerSecond() {
#ifdef OTM_EXTENDED_DEBUG
dumpDecodedBitmap();
#endif
}

// ************************************************************
// For 595ds where the output enable pin works as a blanking pin
// ************************************************************
void OutputManager_::setBlanked(bool blank) {
  if (blank) {
    digitalWrite(BLANKPin, LOW);
  } else {
    digitalWrite(BLANKPin, HIGH);
  }
  blanked = blank;
  if (blanked) {
    debugMsgOtm("Display blanked");
  } else {
    debugMsgOtm("Display unblanked");
  }
}

#ifdef OTM_EXTENDED_DEBUG
void OutputManager_::dumpDecodedBitmap() {
  String val = "Number Array: ";
  for (int i = 0; i < 24; i++) {
    if (dispVal & (1 << (i))) {
      val = val + "1";
    } else {
      val = val + "0";  
    }
    if (i == 10) {
      val = val + " ";
    }
    if (i == 21) {
      val = val + " ";
    }
  }
  if (blanked) {
    val = val + " (BLANKED)";
  } else {
    val = val + " (UNBLANKED)";
  }
  val = val + " Board Count: " + String(dispBoardCount);
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