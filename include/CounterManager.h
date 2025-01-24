#pragma once

#include <FS.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "DebugManager.h"
#include "Globals.h"
#include "utilities.h"

#define VALUES_PER_DIGIT        10
#define REPETITIONS             1
#define FIELD_COUNT_EXPECTED    VALUES_PER_DIGIT + REPETITIONS

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- SPIFFS Clock Component ---------------------------------------
// ----------------------------------------------------------------------------------------------------

class CounterManager_
{
  private:
    CounterManager_() = default; // Make constructor private

  public:
    static CounterManager_ &getInstance(); // Accessor for singleton instance

    CounterManager_(const CounterManager_ &) = delete; // no copying
    CounterManager_ &operator=(const CounterManager_ &) = delete;

  public:
    void setCounterValues(String counterArray);
    void startCounter();
    void stopCounter();
    void restartCounter();
    void counterCount();
    bool isCounterExpired();
    unsigned int getCurrentCounterVal();
  private:
    void copyInitialArrayToCurrent();
    void copyInitialRepetitionsToCurrent();
    String _inputString = "";
    bool _counterRunning = false;
    bool _counterDone = false;
    byte _digitInitialValues[VALUES_PER_DIGIT];
    byte _digitCurrentValues[VALUES_PER_DIGIT];
    int  _repetitionsInitial = 0;
    int  _repetitionsCurrent = 0;
    unsigned int _currentCount = 0;
};

extern CounterManager_ &counterManager;