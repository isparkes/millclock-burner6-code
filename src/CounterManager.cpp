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
// 
// ************************************************************
void CounterManager_::copyInitialRepetitionsToCurrent() {
  _repetitionsCurrent = _repetitionsInitial;
}

// ************************************************************
// Set and parse the counter values from the separated string
// ************************************************************
void CounterManager_::setCounterValues(String inputString) {
  _counterValues = inputString;
  int fieldCountFound = getValueCount(_counterValues, ';');
  if (fieldCountFound != FIELD_COUNT_EXPECTED) {
    debugMsgCmg("Wrong number of fields. Got (" + inputString + ")");
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

    copyInitialArrayToCurrent();
    copyInitialRepetitionsToCurrent();
  }
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
    debugMsgCmg("Counter not running.");
    return;
  }

  unsigned int outputValue = 0;
  int valueIndex = 0;
  int foundValue = 0;

  // Fast forward to the current one we are working on
  while ((valueIndex < 10) && (_digitCurrentValues[valueIndex] == 0)) {
    valueIndex++;
  }

  // did we run off the end?
  if(valueIndex == 10) {
    if (_repetitionsCurrent > 0) {
      _repetitionsCurrent--;
      debugMsgCmg("New repetition: Remaining: " + String(_repetitionsCurrent) + " of " + String(_repetitionsInitial));

      copyInitialArrayToCurrent();
      valueIndex = 0;
    } else {
      // we ran out of repetitions
      _counterRunning = false;
      _counterDone = true;
      foundValue = 0;
    }
  } else {
    foundValue = valueIndex;
    _digitCurrentValues[valueIndex]--;
  }

  #ifdef CMG_EXTENDED_DEBUG
  debugMsgCmg("Value of digit " + String(valueIndex) + " = " + String(foundValue) + " (" + String(_digitCurrentValues[valueIndex]) + ")");
  #endif

  for (int digit = 0 ; digit < DIGIT_COUNT ; digit++) {
    outputValue = outputValue * 10 + foundValue;
  }

  _currentCount = outputValue;
}

// ************************************************************
// 
// ************************************************************
bool CounterManager_::isCounterRunning() {
  return _counterRunning;
}

// ************************************************************
// 
// ************************************************************
bool CounterManager_::isCounterExpired() {
  return _counterDone;
}

// ************************************************************
// 
// ************************************************************
unsigned int CounterManager_::getCurrentCounterVal() {
  return _currentCount;
}


// ************************************************************
// Internal plumbing
// ************************************************************

CounterManager_ &CounterManager_::getInstance() {
  static CounterManager_ instance;
  return instance;
}

CounterManager_ &counterManager = counterManager.getInstance();