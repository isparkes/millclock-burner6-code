#pragma once

#include <FS.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "DebugManager.h"
#include "Globals.h"
#include "utilities.h"

#define VALUES_PER_DIGIT         11
#define FIELD_COUNT_EXPECTED     VALUES_PER_DIGIT+2

// format 11 digit times; use  11th ; repetitions
#define DEFAULT_VALUES_ZIN70     "60;60;60;60;60;60;60;60;60;60;60;1;3"
#define DEFAULT_VALUES_ZIN18     "30;30;30;30;30;30;30;30;30;30;30;0;3"

#define DEFAULT_TUBE_BOARD_COUNT 3

// ----------------------------------------------------------------------------------------------------
// ------------------------------------ Counter Manager Component -------------------------------------
// ----------------------------------------------------------------------------------------------------

typedef struct {
  boolean valid;
  uint digitTime[VALUES_PER_DIGIT];
  bool use11thDigit;
  uint repetitions;
} config_set_t;

class CounterManager_
{
  private:
    CounterManager_() = default; // Make constructor private

  public:
    static CounterManager_ &getInstance(); // Accessor for singleton instance

    CounterManager_(const CounterManager_ &) = delete; // no copying
    CounterManager_ &operator=(const CounterManager_ &) = delete;

  public:
    void setTubeType(tube_type_t newType);
    tube_type_t getTubeType();
    String getTubeTypeAsString();

    void setCounterValues(tube_type_t tubeType, String counterArray);
    String getCounterValues(tube_type_t tubeType);

    // Get the counter values for the current tube type
    String getCounterValuesInitial();
    String getCounterValuesCurrent();

    unsigned int getSecondsRemainingCurrentValue();

    int getRepetitions();
    int getRepetitionsCurrent();
    void setRepetitions(int newRepetitions);
    int  getDigitTime(int digit);
    void setDigitTime(int digit, int newDuration);
    int  getDigitCount();

    void startCounter();
    void stopCounter();
    void pauseCounter();
    void resetCounter();
    void setDigit(byte digit);
    String getNextDigitString();
    void counterCount();
    bool isCounterRunning();
    bool isCounterExpired();
    bool isCounterPaused();
    bool isCounterOverride();
    bool isCounterExpiredConfirmed();
    void confirmCounterExpired();
    unsigned int getCurrentCounterVal();
    String getCurrentCounterValString();
    void toggleCounterRunning();
    void toggleTubeType();
    void incrementOverrideValue();
    void decrementOverrideValue();
  private:
    void copyInitialArrayToCurrent();
    void copyInitialRepetitionsToCurrent();
    config_set_t getCurrentConfigSet();
    config_set_t parseConfigString(String inputString);

    bool _isOverride = false;
    bool _counterRunning = false;
    bool _counterPaused = false;
    bool _counterDone = false;
    bool _counterDoneConfirmed = false;
    byte _digitCurrentValues[VALUES_PER_DIGIT];
    int  _repetitionsCurrent = 0;
    int _overrideCount = 0;
    unsigned int _currentCount = 0;
    tube_type_t _currentTubeType = ZIN70;
    config_set_t configZIN70;
    config_set_t configZIN18;
};

extern CounterManager_ &counterManager;