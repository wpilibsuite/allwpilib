/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogTriggerDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogTriggerData() {
  static AnalogTriggerData satd[kNumAnalogTriggers];
  ::hal::SimAnalogTriggerData = satd;
}
}  // namespace init
}  // namespace hal

AnalogTriggerData* hal::SimAnalogTriggerData;
void AnalogTriggerData::ResetData() {
  m_initialized = 0;
  m_initializedCallbacks = nullptr;
  m_triggerLowerBound = 0;
  m_triggerLowerBoundCallbacks = nullptr;
  m_triggerUpperBound = 0;
  m_triggerUpperBoundCallbacks = nullptr;
  m_triggerMode = static_cast<HALSIM_AnalogTriggerMode>(0);
  m_triggerModeCallbacks = nullptr;
}

int32_t AnalogTriggerData::RegisterInitializedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_initializedCallbacks = RegisterCallback(
        m_initializedCallbacks, "Initialized", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetInitialized());
    callback("Initialized", param, &value);
  }
  return newUid;
}

void AnalogTriggerData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void AnalogTriggerData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool AnalogTriggerData::GetInitialized() { return m_initialized; }

void AnalogTriggerData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t AnalogTriggerData::RegisterTriggerLowerBoundCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_triggerLowerBoundCallbacks =
        RegisterCallback(m_triggerLowerBoundCallbacks, "TriggerLowerBound",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetTriggerLowerBound());
    callback("TriggerLowerBound", param, &value);
  }
  return newUid;
}

void AnalogTriggerData::CancelTriggerLowerBoundCallback(int32_t uid) {
  m_triggerLowerBoundCallbacks =
      CancelCallback(m_triggerLowerBoundCallbacks, uid);
}

void AnalogTriggerData::InvokeTriggerLowerBoundCallback(HAL_Value value) {
  InvokeCallback(m_triggerLowerBoundCallbacks, "TriggerLowerBound", &value);
}

double AnalogTriggerData::GetTriggerLowerBound() { return m_triggerLowerBound; }

void AnalogTriggerData::SetTriggerLowerBound(double triggerLowerBound) {
  double oldValue = m_triggerLowerBound.exchange(triggerLowerBound);
  if (oldValue != triggerLowerBound) {
    InvokeTriggerLowerBoundCallback(MakeDouble(triggerLowerBound));
  }
}

int32_t AnalogTriggerData::RegisterTriggerUpperBoundCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_triggerUpperBoundCallbacks =
        RegisterCallback(m_triggerUpperBoundCallbacks, "TriggerUpperBound",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetTriggerUpperBound());
    callback("TriggerUpperBound", param, &value);
  }
  return newUid;
}

void AnalogTriggerData::CancelTriggerUpperBoundCallback(int32_t uid) {
  m_triggerUpperBoundCallbacks =
      CancelCallback(m_triggerUpperBoundCallbacks, uid);
}

void AnalogTriggerData::InvokeTriggerUpperBoundCallback(HAL_Value value) {
  InvokeCallback(m_triggerUpperBoundCallbacks, "TriggerUpperBound", &value);
}

double AnalogTriggerData::GetTriggerUpperBound() { return m_triggerUpperBound; }

void AnalogTriggerData::SetTriggerUpperBound(double triggerUpperBound) {
  double oldValue = m_triggerUpperBound.exchange(triggerUpperBound);
  if (oldValue != triggerUpperBound) {
    InvokeTriggerUpperBoundCallback(MakeDouble(triggerUpperBound));
  }
}

int32_t AnalogTriggerData::RegisterTriggerModeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_triggerModeCallbacks = RegisterCallback(
        m_triggerModeCallbacks, "TriggerMode", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeEnum(GetTriggerMode());
    callback("TriggerMode", param, &value);
  }
  return newUid;
}

void AnalogTriggerData::CancelTriggerModeCallback(int32_t uid) {
  m_triggerModeCallbacks = CancelCallback(m_triggerModeCallbacks, uid);
}

void AnalogTriggerData::InvokeTriggerModeCallback(HAL_Value value) {
  InvokeCallback(m_triggerModeCallbacks, "TriggerMode", &value);
}

HALSIM_AnalogTriggerMode AnalogTriggerData::GetTriggerMode() {
  return m_triggerMode;
}

void AnalogTriggerData::SetTriggerMode(HALSIM_AnalogTriggerMode triggerMode) {
  HALSIM_AnalogTriggerMode oldValue = m_triggerMode.exchange(triggerMode);
  if (oldValue != triggerMode) {
    InvokeTriggerModeCallback(MakeEnum(triggerMode));
  }
}

extern "C" {
void HALSIM_ResetAnalogTriggerData(int32_t index) {
  SimAnalogTriggerData[index].ResetData();
}

int32_t HALSIM_RegisterAnalogTriggerInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogTriggerData[index].RegisterInitializedCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogTriggerInitializedCallback(int32_t index, int32_t uid) {
  SimAnalogTriggerData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetAnalogTriggerInitialized(int32_t index) {
  return SimAnalogTriggerData[index].GetInitialized();
}

void HALSIM_SetAnalogTriggerInitialized(int32_t index, HAL_Bool initialized) {
  SimAnalogTriggerData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogTriggerData[index].RegisterTriggerLowerBoundCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback(int32_t index,
                                                         int32_t uid) {
  SimAnalogTriggerData[index].CancelTriggerLowerBoundCallback(uid);
}

double HALSIM_GetAnalogTriggerTriggerLowerBound(int32_t index) {
  return SimAnalogTriggerData[index].GetTriggerLowerBound();
}

void HALSIM_SetAnalogTriggerTriggerLowerBound(int32_t index,
                                              double triggerLowerBound) {
  SimAnalogTriggerData[index].SetTriggerLowerBound(triggerLowerBound);
}

int32_t HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogTriggerData[index].RegisterTriggerUpperBoundCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback(int32_t index,
                                                         int32_t uid) {
  SimAnalogTriggerData[index].CancelTriggerUpperBoundCallback(uid);
}

double HALSIM_GetAnalogTriggerTriggerUpperBound(int32_t index) {
  return SimAnalogTriggerData[index].GetTriggerUpperBound();
}

void HALSIM_SetAnalogTriggerTriggerUpperBound(int32_t index,
                                              double triggerUpperBound) {
  SimAnalogTriggerData[index].SetTriggerUpperBound(triggerUpperBound);
}

int32_t HALSIM_RegisterAnalogTriggerTriggerModeCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogTriggerData[index].RegisterTriggerModeCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogTriggerTriggerModeCallback(int32_t index, int32_t uid) {
  SimAnalogTriggerData[index].CancelTriggerModeCallback(uid);
}

HALSIM_AnalogTriggerMode HALSIM_GetAnalogTriggerTriggerMode(int32_t index) {
  return SimAnalogTriggerData[index].GetTriggerMode();
}

void HALSIM_SetAnalogTriggerTriggerMode(int32_t index,
                                        HALSIM_AnalogTriggerMode triggerMode) {
  SimAnalogTriggerData[index].SetTriggerMode(triggerMode);
}

void HALSIM_RegisterAnalogTriggerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  SimAnalogTriggerData[index].RegisterInitializedCallback(callback, param,
                                                          initialNotify);
  SimAnalogTriggerData[index].RegisterTriggerLowerBoundCallback(callback, param,
                                                                initialNotify);
  SimAnalogTriggerData[index].RegisterTriggerUpperBoundCallback(callback, param,
                                                                initialNotify);
  SimAnalogTriggerData[index].RegisterTriggerModeCallback(callback, param,
                                                          initialNotify);
}
}  // extern "C"
