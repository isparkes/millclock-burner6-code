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

#define MAX_CATHODE_COUNT 11
#define CATHODE_COUNT_ZIN70 11
#define CATHODE_COUNT_ZIN18 10

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

    void loadNumberIntegerValue(unsigned int value);

    void setBlanked(bool blank);

    void updateOncePerSecond();
  private:
    uint32_t decodedValue;
    bool blanked = false;

    uint32_t decodeFromNumberArray(byte valueToDecode);
    #ifdef OTM_EXTENDED_DEBUG
    void dumpDecodedBitmap();
    #endif
};

extern OutputManager_ &outputManager;
