/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/PWMData.h"

extern "C" {

void HALSIM_ResetPWMData(int32_t index) {}
int32_t HALSIM_RegisterPWMInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMInitializedCallback(int32_t index, int32_t uid) {}
HAL_Bool HALSIM_GetPWMInitialized(int32_t index) { return false; }
void HALSIM_SetPWMInitialized(int32_t index, HAL_Bool initialized) {}

int32_t HALSIM_RegisterPWMRawValueCallback(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMRawValueCallback(int32_t index, int32_t uid) {}
int32_t HALSIM_GetPWMRawValue(int32_t index) { return 0; }
void HALSIM_SetPWMRawValue(int32_t index, int32_t rawValue) {}

int32_t HALSIM_RegisterPWMSpeedCallback(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMSpeedCallback(int32_t index, int32_t uid) {}
double HALSIM_GetPWMSpeed(int32_t index) { return 0; }
void HALSIM_SetPWMSpeed(int32_t index, double speed) {}

int32_t HALSIM_RegisterPWMPositionCallback(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMPositionCallback(int32_t index, int32_t uid) {}
double HALSIM_GetPWMPosition(int32_t index) { return 0; }
void HALSIM_SetPWMPosition(int32_t index, double position) {}

int32_t HALSIM_RegisterPWMPeriodScaleCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMPeriodScaleCallback(int32_t index, int32_t uid) {}
int32_t HALSIM_GetPWMPeriodScale(int32_t index) { return 0; }
void HALSIM_SetPWMPeriodScale(int32_t index, int32_t periodScale) {}

int32_t HALSIM_RegisterPWMZeroLatchCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelPWMZeroLatchCallback(int32_t index, int32_t uid) {}
HAL_Bool HALSIM_GetPWMZeroLatch(int32_t index) { return false; }
void HALSIM_SetPWMZeroLatch(int32_t index, HAL_Bool zeroLatch) {}

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}

}  // extern "C"
