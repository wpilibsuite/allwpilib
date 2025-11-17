// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/DriverStationData.h"

#include "wpi/hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetDriverStationData(void) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN)                                 \
  HAL_SIMDATAVALUE_STUB_CAPI_NOINDEX(TYPE, HALSIM, DriverStation##CAPINAME, \
                                     RETURN)

DEFINE_CAPI(HAL_Bool, Enabled, false)
DEFINE_CAPI(HAL_Bool, Autonomous, false)
DEFINE_CAPI(HAL_Bool, Test, false)
DEFINE_CAPI(HAL_Bool, EStop, false)
DEFINE_CAPI(HAL_Bool, FmsAttached, false)
DEFINE_CAPI(HAL_Bool, DsAttached, false)
DEFINE_CAPI(HAL_AllianceStationID, AllianceStationId,
            HAL_AllianceStationID_kRed1)
DEFINE_CAPI(double, MatchTime, 0)

#undef DEFINE_CAPI
#define DEFINE_CAPI(name, data)                                                \
  int32_t HALSIM_RegisterJoystick##name##Callback(                             \
      int32_t joystickNum, HAL_Joystick##name##Callback callback, void* param, \
      HAL_Bool initialNotify) {                                                \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  void HALSIM_CancelJoystick##name##Callback(int32_t uid) {}                   \
                                                                               \
  void HALSIM_GetJoystick##name(int32_t joystickNum, HAL_Joystick##name* d) {} \
                                                                               \
  void HALSIM_SetJoystick##name(int32_t joystickNum,                           \
                                const HAL_Joystick##name* d) {}

DEFINE_CAPI(Axes, axes)
DEFINE_CAPI(POVs, povs)
DEFINE_CAPI(Buttons, buttons)
DEFINE_CAPI(Descriptor, descriptor)

int32_t HALSIM_RegisterJoystickLedsCallback(int32_t joystickNum,
                                            HAL_JoystickLedsCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelJoystickLedsCallback(int32_t uid) {}

void HALSIM_GetJoystickLeds(int32_t joystickNum, int32_t* leds) {}

void HALSIM_SetJoystickLeds(int32_t joystickNum, int32_t leds) {}

int32_t HALSIM_RegisterJoystickRumblesCallback(
    int32_t joystickNum, HAL_JoystickRumblesCallback callback, void* param,
    HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelJoystickRumblesCallback(int32_t uid) {}

void HALSIM_GetJoystickRumbles(int32_t joystickNum, int32_t* leftRumble,
                               int32_t* rightRumble, int32_t* leftTriggerRumble,
                               int32_t* rightTriggerRumble) {}

void HALSIM_SetJoystickRumbles(int32_t joystickNum, int32_t leftRumble,
                               int32_t rightRumble, int32_t leftTriggerRumble,
                               int32_t rightTriggerRumble) {}

int32_t HALSIM_RegisterMatchInfoCallback(HAL_MatchInfoCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelMatchInfoCallback(int32_t uid) {}

void HALSIM_GetMatchInfo(HAL_MatchInfo* info) {}

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info) {}

int32_t HALSIM_RegisterDriverStationNewDataCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelDriverStationNewDataCallback(int32_t uid) {}

void HALSIM_NotifyDriverStationNewData(void) {}

void HALSIM_SetJoystickButton(int32_t stick, int32_t button, HAL_Bool state) {}

void HALSIM_SetJoystickAxis(int32_t stick, int32_t axis, double value) {}

void HALSIM_SetJoystickPOV(int32_t stick, int32_t pov, HAL_JoystickPOV value) {}

void HALSIM_SetJoystickButtonsValue(int32_t stick, uint64_t buttons) {}

void HALSIM_SetJoystickAxesAvailable(int32_t stick, uint16_t available) {}

void HALSIM_SetJoystickPOVsAvailable(int32_t stick, uint8_t available) {}

void HALSIM_SetJoystickButtonsAvailable(int32_t stick, uint64_t available) {}

void HALSIM_GetJoystickAvailables(int32_t stick, uint16_t* axesAvailable,
                                  uint64_t* buttonsAvailable,
                                  uint8_t* povsAvailable) {
  *axesAvailable = 0;
  *buttonsAvailable = 0;
  *povsAvailable = 0;
}

void HALSIM_SetJoystickIsGamepad(int32_t stick, HAL_Bool isGamepad) {}

void HALSIM_SetJoystickGamepadType(int32_t stick, int32_t type) {}

void HALSIM_SetJoystickName(int32_t stick, const struct WPI_String* name) {}

void HALSIM_SetJoystickSupportedOutputs(int32_t stick,
                                        int32_t supportedOutputs) {}

void HALSIM_SetGameSpecificMessage(const struct WPI_String* message) {}

void HALSIM_SetEventName(const struct WPI_String* name) {}

void HALSIM_SetMatchType(HAL_MatchType type) {}

void HALSIM_SetMatchNumber(int32_t matchNumber) {}

void HALSIM_SetReplayNumber(int32_t replayNumber) {}

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"
