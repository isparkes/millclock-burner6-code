#include "CounterManager.h"

//**********************************************************************************
//**********************************************************************************
//*                           Digit Counter functions                              *
//**********************************************************************************
//**********************************************************************************

// ************************************************************
// Get the current config set
// ************************************************************
config_set_t CounterManager_::getCurrentConfigSet() {
  config_set_t currentConfigSet;
  switch (_currentTubeType)
  {
  case ZIN70:
    currentConfigSet = configZIN70;
    break;
  case ZIN18:
    currentConfigSet = configZIN18;
    break;
  default:
    currentConfigSet = configZIN70;
    break;
  }

  return currentConfigSet;
}

// ************************************************************
// Reset the current counter values to the initial values 
// ************************************************************
void CounterManager_::copyInitialArrayToCurrent() {
  config_set_t currentConfigSet = getCurrentConfigSet();

  // Copy the initial values to the current values
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    _digitCurrentValues[digit] = currentConfigSet.digitTime[digit];
  }
}

// ************************************************************
// Reset the current repetition values to the initial values
// ************************************************************
void CounterManager_::copyInitialRepetitionsToCurrent() {
  config_set_t currentConfigSet = getCurrentConfigSet();

  _repetitionsCurrent = currentConfigSet.repetitions;
}

// ************************************************************
// Set the current tube type
// ************************************************************
void CounterManager_::setTubeType(tube_type_t newType) {
  _currentTubeType = newType;
  copyInitialArrayToCurrent();
  copyInitialRepetitionsToCurrent();
}

// ************************************************************
// Get the current tube type
// ************************************************************
tube_type_t CounterManager_::getTubeType() {
  return _currentTubeType;
}

// ************************************************************
// Get the current tube type as a string
// ************************************************************
String CounterManager_::getTubeTypeAsString() {
  if( _currentTubeType == ZIN70 ) {
    return "ZIN70";
  } else if ( _currentTubeType == ZIN18 ) {
    return "ZIN18";
  } else {
    return "Unknown";
  }
} 

// ************************************************************
// Set and parse the counter values from the separated string
// ************************************************************
void CounterManager_::setCounterValues(tube_type_t tubeType, String inputString) {
  config_set_t newConfig = parseConfigString(inputString);
  if (!newConfig.valid) {
    debugMsgCmg("Invalid configuration string: " + inputString);
    return;
  }

  if (tubeType == ZIN70) {
    configZIN70 = newConfig;
    debugMsgCmg("Set new configuration for ZIN70 from string: " + inputString);
  } else if (tubeType == ZIN18) {
    configZIN18 = newConfig;
    debugMsgCmg("Set new configuration for ZIN18 from string: " + inputString);
  } else {
    debugMsgCmg("Invalid tube type specified.");
    return;
  }

  // Reset the current values to match the new initial values
  copyInitialArrayToCurrent();
  copyInitialRepetitionsToCurrent();
}

// ************************************************************
// Parse a config set from the separated string
// ************************************************************
config_set_t CounterManager_::parseConfigString(String inputString) {
  config_set_t result;
  int fieldCountFound = getValueCount(inputString, ';');
  if (fieldCountFound != FIELD_COUNT_EXPECTED) {
    debugMsgCmg("Wrong number of fields. Got " + String(fieldCountFound) + " fields from, inputString (" + inputString + "), but was expecting " + String(FIELD_COUNT_EXPECTED) + ".");
    result.valid = false;
    return result;
  }

  // digit times
  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    String valueString = getValueAtIndex(inputString, ';', digit);
    int value = atoi(valueString.c_str());
    debugMsgCmg("Value at index " + String(digit) + " = " + String(value));
    result.digitTime[digit] = value;
  }

  // use 11th digit
  String use11thString = getValueAtIndex(inputString, ';', VALUES_PER_DIGIT);
  bool use11thDigit = atoi(use11thString.c_str());
  debugMsgCmg("use11thDigit: " + String(use11thDigit));
  result.use11thDigit = use11thDigit;
  
  // repetitions
  String repetitionsString = getValueAtIndex(inputString, ';', VALUES_PER_DIGIT + 1);
  int repetitions = atoi(repetitionsString.c_str());
  debugMsgCmg("Repetitions: " + String(repetitions));
  result.repetitions = repetitions;

  // we got to the end, the result looks valid
  result.valid = true;
  return result;
}

// ************************************************************
// Rebuild the counter values string from the current config
// ************************************************************
String CounterManager_::getCounterValues(tube_type_t tubeType) {
  String _counterValues = "";
  config_set_t config;
  if (tubeType == ZIN70) {
    config = configZIN70;
  } else if (tubeType == ZIN18) {
    config = configZIN18;
  } else {
    debugMsgCmg("Invalid tube type specified.");
  }

  if (!config.valid) {
    debugMsgCmg("Configuration for specified tube type is not valid.");
    return "Invalid";
  }

  for (int digit = 0 ; digit < VALUES_PER_DIGIT ; digit++) {
    _counterValues = _counterValues + String(config.digitTime[digit]) + ";";
  }
  _counterValues = _counterValues + String(tubeType == ZIN70 ? "0" : "1") + ";";
  _counterValues = _counterValues + String(config.repetitions);

  return _counterValues;
}

// ************************************************************
// Get the repetition value
// ************************************************************
int CounterManager_::getRepetitions() {
  config_set_t currentConfigSet = getCurrentConfigSet();
  
  return currentConfigSet.repetitions;
}

// ************************************************************
// Set the repetition value
// ************************************************************
void CounterManager_::setRepetitions(int newRepetitions) {
  config_set_t currentConfigSet = getCurrentConfigSet();
  
  switch (_currentTubeType)
  {
  case ZIN70:
  default:
    configZIN70.repetitions = newRepetitions;
    break;
  case ZIN18:
    configZIN18.repetitions = newRepetitions;
    break;
  }
}

// ************************************************************
// Get a single digit value duration
// ************************************************************
int CounterManager_::getDigitTime(int digit) {
  config_set_t currentConfigSet = getCurrentConfigSet();

  if ((digit < 0) || (digit >= VALUES_PER_DIGIT)) {
    debugMsgCmg("Digit index out of range: " + String(digit));
    digit = 0;  // return the first one
  }
  return currentConfigSet.digitTime[digit];
}

// ************************************************************
// Set a single digit value duration
// ************************************************************
void CounterManager_::setDigitTime(int digit, int newDuration) {
  if ((digit < 0) || (digit >= VALUES_PER_DIGIT)) {
    debugMsgCmg("Digit index out of range: " + String(digit));
    return;
  }
  debugMsgCmg("Set digit: " + String(digit) + " to duration: " + String(newDuration));
  switch (_currentTubeType)
  {
  case ZIN70:
  default:
    configZIN70.digitTime[digit] = newDuration;
    break;
  case ZIN18:
    configZIN18.digitTime[digit] = newDuration;
    break;
  }

  copyInitialArrayToCurrent();
}

// ************************************************************
// Get a single digit value duration
// ************************************************************
int CounterManager_::getDigitCount() {
  if (_currentTubeType == ZIN70) {
    return VALUES_PER_DIGIT;
  } else {
    return VALUES_PER_DIGIT - 1;
  }
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
// Start/restart the counter
// ************************************************************
void CounterManager_::startCounter() {
  if(_isOverride) {
    debugMsgCmg("Clearing override mode on counter start.");
    clearUpToCurrentOverrideDigit();
    _isOverride = false;
  }
  debugMsgCmg("!!!Starting Counter");
  _counterRunning = true;
  _counterPaused = false;
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
  _counterPaused = false;
}

// ************************************************************
// Stop the counter
// ************************************************************
void CounterManager_::stopCounter() {
  debugMsgCmg("!!!Stopping Counter");
  _counterRunning = false;
  _counterPaused = false;
}

// ************************************************************
// Pause the counter
// ************************************************************
void CounterManager_::pauseCounter() {
  if (!_counterPaused) {
    debugMsgCmg("!!!Pausing Counter");
  }
  _counterPaused = true;
}

// ************************************************************
// Start/restart the counter
// ************************************************************
void CounterManager_::incrementOverrideValue() {
  if (!_isOverride) {
    debugMsgCmg("Setting override value");
    _overrideCount = _currentCount;
    _isOverride = true;
  } else {
    // Determine max value based on tube type
    int maxVal = (_currentTubeType == ZIN70) ? 10 : 9;

    // Clamp to max (don't wrap)
    if (_overrideCount < maxVal) {
      _overrideCount++;
      _currentCount = _overrideCount;
    }
  }
}

// ************************************************************
// Start/restart the counter
// ************************************************************
void CounterManager_::decrementOverrideValue() {
  if (!_isOverride) {
    debugMsgCmg("Setting override value");
    _overrideCount = _currentCount;
    _isOverride = true;
  } else {
    // Clamp to min (don't wrap)
    if (_overrideCount > 0) {
      _overrideCount--;
      _currentCount = _overrideCount;
    }
  }
}

// ************************************************************
// Clear up to current override digit
// ************************************************************
void CounterManager_::clearUpToCurrentOverrideDigit() {
  debugMsgCmg("Clearing up to digit: " + String(_overrideCount));
  for (int digit = 0 ; digit < _overrideCount ; digit++) {
    _digitCurrentValues[digit] = 0;
  }
}

// ************************************************************
// Do a count - called once per second
// ************************************************************
void CounterManager_::counterCount() {
  if (_isOverride) {
    #ifdef CMG_EXTENDED_DEBUG
    debugMsgCmg("Override active - skipping count.");
    #endif
    _currentCount = _overrideCount;
    return;
  }

  if ((!_counterRunning) || (_counterPaused)) {
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
      debugMsgCmg("New repetition: Remaining: " + String(_repetitionsCurrent) + " of " + String(getRepetitions()));

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
// Is the counter paused?
// ************************************************************
bool CounterManager_::isCounterPaused() {
  return _counterPaused;
}

// ************************************************************
// Is the counter in override mode?
// ************************************************************
bool CounterManager_::isCounterOverride() {
  return _isOverride;
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
  } else if(getCurrentCounterVal() == 10) {
    returnVal = "K";
  } else {
    debugMsgCmg("Digit value out of range: " + String(getCurrentCounterVal()));
    returnVal = "0";
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
// Button press toggle of running status
// ************************************************************
void CounterManager_::toggleTubeType() {
    switch (_currentTubeType) {
    case ZIN70: {
      setTubeType(ZIN18);
      break;
    }
    case ZIN18: {
      setTubeType(ZIN70);
      break;
    }
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