/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/DIOData.h"

extern "C" {

void HALSIM_ResetDIOData(int32_t index) {}
int32_t HALSIM_RegisterDIOInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelDIOInitializedCallback(int32_t index, int32_t uid) {}
HAL_Bool HALSIM_GetDIOInitialized(int32_t index) { return false; }
void HALSIM_SetDIOInitialized(int32_t index, HAL_Bool initialized) {}

int32_t HALSIM_RegisterDIOValueCallback(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelDIOValueCallback(int32_t index, int32_t uid) {}
HAL_Bool HALSIM_GetDIOValue(int32_t index) { return false; }
void HALSIM_SetDIOValue(int32_t index, HAL_Bool value) {}

int32_t HALSIM_RegisterDIOPulseLengthCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelDIOPulseLengthCallback(int32_t index, int32_t uid) {}
double HALSIM_GetDIOPulseLength(int32_t index) { return 0; }
void HALSIM_SetDIOPulseLength(int32_t index, double pulseLength) {}

int32_t HALSIM_RegisterDIOIsInputCallback(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelDIOIsInputCallback(int32_t index, int32_t uid) {}
HAL_Bool HALSIM_GetDIOIsInput(int32_t index) { return false; }
void HALSIM_SetDIOIsInput(int32_t index, HAL_Bool isInput) {}

int32_t HALSIM_RegisterDIOFilterIndexCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelDIOFilterIndexCallback(int32_t index, int32_t uid) {}
int32_t HALSIM_GetDIOFilterIndex(int32_t index) { return 0; }
void HALSIM_SetDIOFilterIndex(int32_t index, int32_t filterIndex) {}

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}

}  // extern "C"
