/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogGyroDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogGyroData() {
  static AnalogGyroData agd[kNumAccumulators];
  ::hal::SimAnalogGyroData = agd;
}
}  // namespace init
}  // namespace hal

AnalogGyroData* hal::SimAnalogGyroData;
void AnalogGyroData::ResetData() {
  m_angle = 0.0;
  m_angleCallbacks = nullptr;
  m_rate = 0.0;
  m_rateCallbacks = nullptr;
  m_initialized = false;
  m_initializedCallbacks = nullptr;
}

int32_t AnalogGyroData::RegisterAngleCallback(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_angleCallbacks =
        RegisterCallback(m_angleCallbacks, "Angle", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetAngle());
    callback("Angle", param, &value);
  }
  return newUid;
}

void AnalogGyroData::CancelAngleCallback(int32_t uid) {
  m_angleCallbacks = CancelCallback(m_angleCallbacks, uid);
}

void AnalogGyroData::InvokeAngleCallback(HAL_Value value) {
  InvokeCallback(m_angleCallbacks, "Angle", &value);
}

double AnalogGyroData::GetAngle() { return m_angle; }

void AnalogGyroData::SetAngle(double angle) {
  double oldValue = m_angle.exchange(angle);
  if (oldValue != angle) {
    InvokeAngleCallback(MakeDouble(angle));
  }
}

int32_t AnalogGyroData::RegisterRateCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_rateCallbacks =
        RegisterCallback(m_rateCallbacks, "Rate", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetRate());
    callback("Rate", param, &value);
  }
  return newUid;
}

void AnalogGyroData::CancelRateCallback(int32_t uid) {
  m_rateCallbacks = CancelCallback(m_rateCallbacks, uid);
}

void AnalogGyroData::InvokeRateCallback(HAL_Value value) {
  InvokeCallback(m_rateCallbacks, "Rate", &value);
}

double AnalogGyroData::GetRate() { return m_rate; }

void AnalogGyroData::SetRate(double rate) {
  double oldValue = m_rate.exchange(rate);
  if (oldValue != rate) {
    InvokeRateCallback(MakeDouble(rate));
  }
}

int32_t AnalogGyroData::RegisterInitializedCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
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

void AnalogGyroData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void AnalogGyroData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool AnalogGyroData::GetInitialized() { return m_initialized; }

void AnalogGyroData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

extern "C" {
void HALSIM_ResetAnalogGyroData(int32_t index) {
  SimAnalogGyroData[index].ResetData();
}

int32_t HALSIM_RegisterAnalogGyroAngleCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  return SimAnalogGyroData[index].RegisterAngleCallback(callback, param,
                                                        initialNotify);
}

void HALSIM_CancelAnalogGyroAngleCallback(int32_t index, int32_t uid) {
  SimAnalogGyroData[index].CancelAngleCallback(uid);
}

double HALSIM_GetAnalogGyroAngle(int32_t index) {
  return SimAnalogGyroData[index].GetAngle();
}

void HALSIM_SetAnalogGyroAngle(int32_t index, double angle) {
  SimAnalogGyroData[index].SetAngle(angle);
}

int32_t HALSIM_RegisterAnalogGyroRateCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimAnalogGyroData[index].RegisterRateCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelAnalogGyroRateCallback(int32_t index, int32_t uid) {
  SimAnalogGyroData[index].CancelRateCallback(uid);
}

double HALSIM_GetAnalogGyroRate(int32_t index) {
  return SimAnalogGyroData[index].GetRate();
}

void HALSIM_SetAnalogGyroRate(int32_t index, double rate) {
  SimAnalogGyroData[index].SetRate(rate);
}

int32_t HALSIM_RegisterAnalogGyroInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogGyroData[index].RegisterInitializedCallback(callback, param,
                                                              initialNotify);
}

void HALSIM_CancelAnalogGyroInitializedCallback(int32_t index, int32_t uid) {
  SimAnalogGyroData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetAnalogGyroInitialized(int32_t index) {
  return SimAnalogGyroData[index].GetInitialized();
}

void HALSIM_SetAnalogGyroInitialized(int32_t index, HAL_Bool initialized) {
  SimAnalogGyroData[index].SetInitialized(initialized);
}

void HALSIM_RegisterAnalogGyroAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  SimAnalogGyroData[index].RegisterAngleCallback(callback, param,
                                                 initialNotify);
  SimAnalogGyroData[index].RegisterRateCallback(callback, param, initialNotify);
  SimAnalogGyroData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}
}  // extern "C"
