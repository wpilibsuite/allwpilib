// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/DriverStationTypes.h"
#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

typedef void (*HAL_JoystickAxesCallback)(const char* name, void* param,
                                         int32_t joystickNum,
                                         const HAL_JoystickAxes* axes);
typedef void (*HAL_JoystickPOVsCallback)(const char* name, void* param,
                                         int32_t joystickNum,
                                         const HAL_JoystickPOVs* povs);
typedef void (*HAL_JoystickButtonsCallback)(const char* name, void* param,
                                            int32_t joystickNum,
                                            const HAL_JoystickButtons* buttons);
typedef void (*HAL_JoystickDescriptorCallback)(
    const char* name, void* param, int32_t joystickNum,
    const HAL_JoystickDescriptor* descriptor);
typedef void (*HAL_JoystickOutputsCallback)(const char* name, void* param,
                                            int32_t joystickNum,
                                            int64_t outputs, int32_t leftRumble,
                                            int32_t rightRumble);
typedef void (*HAL_MatchInfoCallback)(const char* name, void* param,
                                      const HAL_MatchInfo* info);

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetDriverStationData(void);
int32_t HALSIM_RegisterDriverStationEnabledCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEnabledCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEnabled(void);
void HALSIM_SetDriverStationEnabled(HAL_Bool enabled);

int32_t HALSIM_RegisterDriverStationAutonomousCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAutonomousCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationAutonomous(void);
void HALSIM_SetDriverStationAutonomous(HAL_Bool autonomous);

int32_t HALSIM_RegisterDriverStationTestCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelDriverStationTestCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationTest(void);
void HALSIM_SetDriverStationTest(HAL_Bool test);

int32_t HALSIM_RegisterDriverStationEStopCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEStopCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEStop(void);
void HALSIM_SetDriverStationEStop(HAL_Bool eStop);

int32_t HALSIM_RegisterDriverStationFmsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationFmsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationFmsAttached(void);
void HALSIM_SetDriverStationFmsAttached(HAL_Bool fmsAttached);

int32_t HALSIM_RegisterDriverStationDsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationDsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationDsAttached(void);
void HALSIM_SetDriverStationDsAttached(HAL_Bool dsAttached);

int32_t HALSIM_RegisterDriverStationAllianceStationIdCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAllianceStationIdCallback(int32_t uid);
HAL_AllianceStationID HALSIM_GetDriverStationAllianceStationId(void);
void HALSIM_SetDriverStationAllianceStationId(
    HAL_AllianceStationID allianceStationId);

int32_t HALSIM_RegisterDriverStationMatchTimeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationMatchTimeCallback(int32_t uid);
double HALSIM_GetDriverStationMatchTime(void);
void HALSIM_SetDriverStationMatchTime(double matchTime);

int32_t HALSIM_RegisterJoystickAxesCallback(int32_t joystickNum,
                                            HAL_JoystickAxesCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
void HALSIM_CancelJoystickAxesCallback(int32_t uid);
void HALSIM_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);
void HALSIM_SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes);

int32_t HALSIM_RegisterJoystickPOVsCallback(int32_t joystickNum,
                                            HAL_JoystickPOVsCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
void HALSIM_CancelJoystickPOVsCallback(int32_t uid);
void HALSIM_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);
void HALSIM_SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs);

int32_t HALSIM_RegisterJoystickButtonsCallback(
    int32_t joystickNum, HAL_JoystickButtonsCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelJoystickButtonsCallback(int32_t uid);
void HALSIM_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons);
void HALSIM_SetJoystickButtons(int32_t joystickNum,
                               const HAL_JoystickButtons* buttons);

int32_t HALSIM_RegisterJoystickDescriptorCallback(
    int32_t joystickNum, HAL_JoystickDescriptorCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelJoystickDescriptorCallback(int32_t uid);
void HALSIM_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* descriptor);
void HALSIM_SetJoystickDescriptor(int32_t joystickNum,
                                  const HAL_JoystickDescriptor* descriptor);

int32_t HALSIM_RegisterJoystickOutputsCallback(
    int32_t joystickNum, HAL_JoystickOutputsCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelJoystickOutputsCallback(int32_t uid);
void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble);
void HALSIM_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble);

int32_t HALSIM_RegisterMatchInfoCallback(HAL_MatchInfoCallback callback,
                                         void* param, HAL_Bool initialNotify);
void HALSIM_CancelMatchInfoCallback(int32_t uid);
void HALSIM_GetMatchInfo(HAL_MatchInfo* info);
void HALSIM_SetMatchInfo(const HAL_MatchInfo* info);

void HALSIM_SetJoystickButton(int32_t stick, int32_t button, HAL_Bool state);
void HALSIM_SetJoystickAxis(int32_t stick, int32_t axis, double value);
void HALSIM_SetJoystickPOV(int32_t stick, int32_t pov, int32_t value);
void HALSIM_SetJoystickButtonsValue(int32_t stick, uint32_t buttons);
void HALSIM_SetJoystickAxisCount(int32_t stick, int32_t count);
void HALSIM_SetJoystickPOVCount(int32_t stick, int32_t count);
void HALSIM_SetJoystickButtonCount(int32_t stick, int32_t count);
void HALSIM_GetJoystickCounts(int32_t stick, int32_t* axisCount,
                              int32_t* buttonCount, int32_t* povCount);

void HALSIM_SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox);
void HALSIM_SetJoystickType(int32_t stick, int32_t type);
void HALSIM_SetJoystickName(int32_t stick, const char* name);
void HALSIM_SetJoystickAxisType(int32_t stick, int32_t axis, int32_t type);

void HALSIM_SetGameSpecificMessage(const char* message);
void HALSIM_SetEventName(const char* name);
void HALSIM_SetMatchType(HAL_MatchType type);
void HALSIM_SetMatchNumber(int32_t matchNumber);
void HALSIM_SetReplayNumber(int32_t replayNumber);

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);

int32_t HALSIM_RegisterDriverStationNewDataCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelDriverStationNewDataCallback(int32_t uid);
void HALSIM_NotifyDriverStationNewData(void);

#ifdef __cplusplus
}  // extern "C"
#endif
