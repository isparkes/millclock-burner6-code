#include "CounterManager.h"

//**********************************************************************************
//**********************************************************************************
//*                           Digit Counter functions                              *
//**********************************************************************************
//**********************************************************************************

void CounterManager_::copyInitialArrayToCurrent() {
  // Copy the initial values to the current values
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    _digitCurrentValues[digit] = _digitInitialValues[digit];
  }
}

void CounterManager_::copyInitialRepetitionsToCurrent() {
  _repetitionsCurrent = _repetitionsInitial;
}

// ************************************************************
// 
// ************************************************************
void CounterManager_::setCounterValues(String inputString) {
  _inputString = inputString;
  int fieldCountFound = getValueCount(_inputString, ';');
  if (fieldCountFound != FIELD_COUNT_EXPECTED) {
    debugMsgCmg("Wrong number of fields");
  } else {
    int index = 0;

    // Digits
    for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
      String valueString = getValueAtIndex(_inputString, ';', index);
      int value = atoi(valueString.c_str());
      index++;
      debugMsgCmg("Value at index " + String(index) + " = " + String(value));
      _digitInitialValues[digit] = value;
    }

    // Repetitons
    String valueString = getValueAtIndex(_inputString, ';', index);
    _repetitionsInitial = atoi(valueString.c_str());
    index++;
    debugMsgCmg("Repetitions: " + String(_repetitionsInitial));
    _counterRunning = false;

    copyInitialArrayToCurrent();
    copyInitialRepetitionsToCurrent();
  }
}

// ************************************************************
// 
// ************************************************************
void CounterManager_::startCounter() {
  debugMsgCmg("!!!Starting Counter");
  copyInitialArrayToCurrent();
  copyInitialRepetitionsToCurrent();
  _counterRunning = true;
}

// ************************************************************
// 
// ************************************************************
void CounterManager_::restartCounter() {
  debugMsgCmg("!!!Restarting Counter");
  _counterRunning = true;
}

// ************************************************************
// 
// ************************************************************
void CounterManager_::stopCounter() {
  debugMsgCmg("!!!Stopping Counter");
  _counterRunning = false;
}

// ************************************************************
// 
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
    debugMsgCmg("Repetitions remaining: " + String(_repetitionsCurrent) + " of " + String(_repetitionsInitial));
    if (_repetitionsCurrent > 0) {
      _repetitionsCurrent--;
      debugMsgCmg("New repetition");

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

  debugMsgCmg("Value of digit " + String(valueIndex) + " = " + String(foundValue) + " (" + String(_digitCurrentValues[valueIndex]) + ")");

  for (int digit = 0 ; digit < DIGIT_COUNT ; digit++) {
    outputValue = outputValue * 10 + foundValue;
  }

  _currentCount = outputValue;
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