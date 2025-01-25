#pragma once

#include <FS.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "DebugManager.h"
#include "Globals.h"
#include "utilities.h"

#define VALUES_PER_DIGIT        10
#define FIELD_COUNT_EXPECTED    VALUES_PER_DIGIT

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
    String getCounterValues();
    String getCounterValuesCurrent();

    int getRepetitions();
    void setRepetitions(int inputRepetitions);
    int getRepetitionsCurrent();

    void startCounter();
    void stopCounter();
    void resetCounter();
    void counterCount();
    bool isCounterRunning();
    bool isCounterExpired();
    unsigned int getCurrentCounterVal();
  private:
    void copyInitialArrayToCurrent();
    void copyInitialRepetitionsToCurrent();
    String _counterValues = "";
    bool _counterRunning = false;
    bool _counterDone = false;
    byte _digitInitialValues[VALUES_PER_DIGIT];
    byte _digitCurrentValues[VALUES_PER_DIGIT];
    int  _repetitionsInitial = 0;
    int  _repetitionsCurrent = 0;
    unsigned int _currentCount = 0;
};

extern CounterManager_ &counterManager;