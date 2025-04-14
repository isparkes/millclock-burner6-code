#pragma once

#include <FS.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "DebugManager.h"
#include "Globals.h"
#include "utilities.h"

#define VALUES_PER_DIGIT        11
#define FIELD_COUNT_EXPECTED    VALUES_PER_DIGIT
#define DEFAULT_COUNTER_VALUES  "60;60;60;60;60;60;60;60;60;60;60;"
#define DEFAULT_REPETITIONS     1

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
    unsigned int getSecondsRemainingCurrentValue();

    int getRepetitions();
    void setRepetitions(int inputRepetitions);
    int getRepetitionsCurrent();

    void startCounter();
    void stopCounter();
    void resetCounter();
    void counterCount();
    bool isCounterRunning();
    bool isCounterExpired();
    bool isCounterExpiredConfirmed();
    void confirmCounterExpired();
    unsigned int getCurrentCounterVal();
    String getCurrentCounterValString();
    void toggleCounterRunning();
  private:
    void copyInitialArrayToCurrent();
    void copyInitialRepetitionsToCurrent();
    void loadDefaultValues();
    String _counterValues = "";
    bool _counterRunning = false;
    bool _counterDone = false;
    bool _counterDoneConfirmed = false;
    byte _digitInitialValues[VALUES_PER_DIGIT];
    byte _digitCurrentValues[VALUES_PER_DIGIT];
    int  _repetitionsInitial = 0;
    int  _repetitionsCurrent = 0;
    unsigned int _currentCount = 0;
};

extern CounterManager_ &counterManager;