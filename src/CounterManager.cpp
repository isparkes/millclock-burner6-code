#include "CounterManager.h"

//**********************************************************************************
//**********************************************************************************
//*                           Digit Counter functions                              *
//**********************************************************************************
//**********************************************************************************

// ************************************************************
// Reset the current counter values to the initial values 
// ************************************************************
void CounterManager_::copyInitialArrayToCurrent() {
  // Copy the initial values to the current values
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    _digitCurrentValues[digit] = _digitInitialValues[digit];
  }
}

// ************************************************************
// Reset the current repetition values to the initial values
// ************************************************************
void CounterManager_::copyInitialRepetitionsToCurrent() {
  _repetitionsCurrent = _repetitionsInitial;
}

// ************************************************************
// Load the default values
// This is used when the values are not set or are invalid
// ************************************************************
void CounterManager_::loadDefaultValues() {
  debugMsgCmg("Loading default values");
  _counterValues = DEFAULT_COUNTER_VALUES;
  _repetitionsInitial = DEFAULT_REPETITIONS;
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    _digitInitialValues[digit] = 0;
  }
}

// ************************************************************
// Set and parse the counter values from the separated string
// ************************************************************
void CounterManager_::setCounterValues(String inputString) {
  _counterValues = inputString;
  int fieldCountFound = getValueCount(_counterValues, ';');
  if (fieldCountFound != FIELD_COUNT_EXPECTED) {
    debugMsgCmg("Wrong number of fields. Got " + String(fieldCountFound) + " fields from, inputString (" + inputString + "), but was expecting " + String(FIELD_COUNT_EXPECTED) + ". Loading default values: (" + String(DEFAULT_COUNTER_VALUES) + ")");
    _counterValues = DEFAULT_COUNTER_VALUES;
    loadDefaultValues();
    spiffsStorage.saveConfigToSpiffs();
  } else {
    int index = 0;

    // Digits
    for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
      String valueString = getValueAtIndex(_counterValues, ';', index);
      int value = atoi(valueString.c_str());
      index++;
      debugMsgCmg("Value at index " + String(index) + " = " + String(value));
      _digitInitialValues[digit] = value;
    }

    // Repetitons
    String valueString = getValueAtIndex(_counterValues, ';', index);
    _repetitionsInitial = atoi(valueString.c_str());
    index++;
    debugMsgCmg("Repetitions: " + String(_repetitionsInitial));
    _counterRunning = false;
  }

  copyInitialArrayToCurrent();
  copyInitialRepetitionsToCurrent();
}

// ************************************************************
// Set and parse the counter values from the separated string
// ************************************************************
String CounterManager_::getCounterValues() {
  return _counterValues;
}

// ************************************************************
// Get the repetition value
// ************************************************************
int CounterManager_::getRepetitions() {
  return _repetitionsInitial;
}

// ************************************************************
// Set and parse the counter values from the separated string
// ************************************************************
String CounterManager_::getCounterValuesCurrent() {
  String returnVal = "";
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    returnVal = returnVal + String(_digitCurrentValues[digit]) + ";";
  }
  return returnVal;
}

// ************************************************************
// Get the repetition value
// ************************************************************
int CounterManager_::getRepetitionsCurrent() {
  return _repetitionsCurrent;
}

// ************************************************************
// Set the repetition value
// ************************************************************
void CounterManager_::setRepetitions(int inputRepetitions) {
  _repetitionsInitial = inputRepetitions;
  copyInitialRepetitionsToCurrent();
}

// ************************************************************
// Start/restart the counter
// ************************************************************
void CounterManager_::startCounter() {
  debugMsgCmg("!!!Starting Counter");
  _counterRunning = true;
}

// ************************************************************
// 
// ************************************************************
void CounterManager_::resetCounter() {
  debugMsgCmg("!!!Restarting Counter");
  copyInitialArrayToCurrent();
  copyInitialRepetitionsToCurrent();
  _counterRunning = true;
  _counterDone = false;
  _counterDoneConfirmed = false;
}

// ************************************************************
// Stop/pause the counter
// ************************************************************
void CounterManager_::stopCounter() {
  debugMsgCmg("!!!Stopping Counter");
  _counterRunning = false;
}

// ************************************************************
// Do a count - called once per second
// ************************************************************
void CounterManager_::counterCount() {
  if (!_counterRunning) {
    #ifdef CMG_EXTENDED_DEBUG
    debugMsgCmg("Counter not running.");
    #endif
    return;
  }

  int valueIndex = 0;
  int foundValue = 0;

  // Fast forward to the current one we are working on
  while ((valueIndex < VALUES_PER_DIGIT) && (_digitCurrentValues[valueIndex] == 0)) {
    valueIndex++;
  }

  // did we run off the end?
  if(valueIndex == VALUES_PER_DIGIT) {
    if (_repetitionsCurrent > 1) {
      _repetitionsCurrent--;
      debugMsgCmg("New repetition: Remaining: " + String(_repetitionsCurrent) + " of " + String(_repetitionsInitial));

      copyInitialArrayToCurrent();
      valueIndex = 0;
    } else {
      // we ran out of repetitions
      _counterRunning = false;
      _counterDone = true;
      foundValue = 0;
      _repetitionsCurrent = 0;
    }
  } else {
    foundValue = valueIndex;
    _digitCurrentValues[valueIndex]--;
  }

  #ifdef CMG_EXTENDED_DEBUG
  debugMsgCmg("Value of digit " + String(valueIndex) + " = " + String(foundValue) + " (" + String(_digitCurrentValues[valueIndex]) + ")");
  #endif

  _currentCount = foundValue;
}

// ************************************************************
// Is the counter running or stopped?
// ************************************************************
bool CounterManager_::isCounterRunning() {
  return _counterRunning;
}

// ************************************************************
// Is the counter expired?
// ************************************************************
bool CounterManager_::isCounterExpired() {
  return _counterDone && !_counterDoneConfirmed;
}

// ************************************************************
// Is the counter expired?
// ************************************************************
bool CounterManager_::isCounterExpiredConfirmed() {
  return _counterDoneConfirmed;
}

// ************************************************************
// Confirm a counter expired event
// ************************************************************
void CounterManager_::confirmCounterExpired() {
  _counterDoneConfirmed = true;
}

// ************************************************************
// Get the current counter value
// ************************************************************
unsigned int CounterManager_::getCurrentCounterVal() {
  return _currentCount;
}

// ************************************************************
// Get the seconds remaining on the current counter value
// ************************************************************
unsigned int CounterManager_::getSecondsRemainingCurrentValue() {
  unsigned int returnVal = 0;
  returnVal = _digitCurrentValues[_currentCount];
  return returnVal;
}

// ************************************************************
// Get value of the digit we are showing
// ************************************************************
String CounterManager_::getCurrentCounterValString() {
  String returnVal = "";
  if (getCurrentCounterVal() < 10) {
    returnVal = String(getCurrentCounterVal());
  } else {
    returnVal = "M";
  }
  return returnVal;
}

// ************************************************************
// Button press toggle of running status
// ************************************************************
void CounterManager_::toggleCounterRunning() {
  _counterRunning = !_counterRunning;
  if (_counterRunning) {
    startCounter();
  } else {
    stopCounter();
  }
}

// ************************************************************
// Internal plumbing
// ************************************************************

CounterManager_ &CounterManager_::getInstance() {
  static CounterManager_ instance;
  return instance;
}

CounterManager_ &counterManager = counterManager.getInstance();