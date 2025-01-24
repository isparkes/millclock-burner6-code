#pragma once

#include <Arduino.h>
#include "Configuration.h"

typedef void (*DebugCallback) (String);

// Extended debug settings - these allow trace level debugging
#define OTM_EXTENDED_DEBUG
#define WFM_EXTENDED_DEBUG_OFF
#define SPF_EXTENDED_DEBUG_OFF
#define CDM_EXTENDED_DEBUG_OFF
#define BLK_EXTENDED_DEBUG_OFF

// Basic debug settings
#ifdef DEBUG
#define debugMsgMain(message) debugManager.debugMsg("[LNC]", message);
#define debugMsgOtm(message) debugManager.debugMsg("[OTM]", message);
#define debugMsgSpf(message) debugManager.debugMsg("[SPF]", message);
#define debugMsgWbm(message) debugManager.debugMsg("[WEB]", message);
#define debugMsgUtl(message) debugManager.debugMsg("[UTL]", message);
#define debugMsgWfm(message) debugManager.debugMsg("[WFM]", message);
#define debugMsgCdm(message) debugManager.debugMsg("[CDM]", message);
#else
#define debugMsgMain(message)
#define debugMsgOtm(message)
#define debugMsgSpf(message)
#define debugMsgWbm(message)
#define debugMsgUtl(message)
#define debugMsgWfm(message)
#define debugMsgCdm(message)
#endif

// Extended debug settings
#ifdef SPF_EXTENDED_DEBUG
#define debugMsgSpfX(message) debugManager.debugMsg("[UTL]", message);
#else
#define debugMsgSpfX(message)
#endif


class DebugManager_ {
  private:
    DebugManager_() = default; // Make constructor private

  public:
    static DebugManager_ &getInstance(); // Accessor for singleton instance

    DebugManager_(const DebugManager_ &) = delete; // no copying
    DebugManager_ &operator=(const DebugManager_ &) = delete;

  public:
    void setDebuggingOutput(bool newState);
    void debugMsg(String prefix, String message);
    void debugMsgCont(String message);
    void setDebugAutoOff(unsigned int seconds);
    void debugAutoOffCheck();
    bool isDebugOn();

    // Some components need to use a callback
    DebugCallback getDebugCallBack();

  private:
    bool _state = true;
    unsigned int _debugForSecs = 0;
};

// free function link to the class function
extern void debugManagerLink(String message);

extern DebugManager_ &debugManager;