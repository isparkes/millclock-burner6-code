#pragma once

#include "Arduino.h"
#include "Globals.h"
#include "utilities.h"
#include "Defs.h"
#include "DebugManager.h"

// -------------------------------------------------------------------------------
// Display mode, set per digit
#define BLANKED  0
#define NORMAL   1
#define BLINK    2

// -------------------------------------------------------------------------------

// The mode selection works on a priority scheme
// The lower priority mode will be diplayed if enabled
// ACP and Diags mode override all other modes
// Slots mode means that the secondary display mode overrides the primary mode
// However value mode, if set via an arbitrary value being set, also overrides the primary mode
// Primary mode is shown when none of the other modes is set, i.e. normally

// These modes can be set as primary or secondary: Time / Date / Value / Countdown

// Value mode when set by a REST push can override the primary mode

typedef void (*DebugCallback) (String);

typedef enum {
    digit0 = 0,
    digit1 = 1,
    digit2 = 2,
    digit3 = 3,
    digit4 = 4,
    digit5 = 5,
    digit6 = 6,
    digit7 = 7,
    digit8 = 8,
    digit9 = 9
} clock_digit;

class OutputManager_ {
  private:
    OutputManager_() = default; // Make constructor private

  public:
    static OutputManager_ &getInstance(); // Accessor for singleton instance

    OutputManager_(const OutputManager_ &) = delete; // no copying
    OutputManager_ &operator=(const OutputManager_ &) = delete;

  public:
    // Do a display impression
    void outputDisplay();

    void loadNumberArraySameValue(byte value);
    void loadNumberArrayIntegerValue(unsigned int value);

    void updateOncePerSecond();

    void setArbitraryValue(unsigned int value);
    void setArbitraryValueDisplayTime(unsigned int value);

    byte getCurrentDisplayDigitValue(byte digit);

  private:
    clock_digit numberArray[DIGIT_COUNT]     = {digit0, digit0, digit0, digit0, digit0, digit0};

    clock_digit convertToDigit(int value);
    void dumpNumberArrayValues();

    uint32_t decodeFromNumberArray(byte valueToDecodeTens, byte valueToDecodeUnits, bool blankTens, bool bankUnits, bool blankSeparators, bool bl1, bool bl2, bool led1, bool led2);
};

extern OutputManager_ &outputManager;
