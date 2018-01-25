/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "PWMDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePWMData() {
  static PWMData spd[kNumPWMChannels];
  ::hal::SimPWMData = spd;
}
}  // namespace init
}  // namespace hal

PWMData* hal::SimPWMData;
void PWMData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_rawValue = 0;
  m_rawValueCallbacks = nullptr;
  m_speed = 0;
  m_speedCallbacks = nullptr;
  m_position = 0;
  m_positionCallbacks = nullptr;
  m_periodScale = 0;
  m_periodScaleCallbacks = nullptr;
  m_zeroLatch = false;
  m_zeroLatchCallbacks = nullptr;
}

int32_t PWMData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void PWMData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void PWMData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool PWMData::GetInitialized() { return m_initialized; }

void PWMData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t PWMData::RegisterRawValueCallback(HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_rawValueCallbacks = RegisterCallback(m_rawValueCallbacks, "RawValue",
                                           callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetRawValue());
    callback("RawValue", param, &value);
  }
  return newUid;
}

void PWMData::CancelRawValueCallback(int32_t uid) {
  m_rawValueCallbacks = CancelCallback(m_rawValueCallbacks, uid);
}

void PWMData::InvokeRawValueCallback(HAL_Value value) {
  InvokeCallback(m_rawValueCallbacks, "RawValue", &value);
}

int32_t PWMData::GetRawValue() { return m_rawValue; }

void PWMData::SetRawValue(int32_t rawValue) {
  int32_t oldValue = m_rawValue.exchange(rawValue);
  if (oldValue != rawValue) {
    InvokeRawValueCallback(MakeInt(rawValue));
  }
}

int32_t PWMData::RegisterSpeedCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_speedCallbacks =
        RegisterCallback(m_speedCallbacks, "Speed", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetSpeed());
    callback("Speed", param, &value);
  }
  return newUid;
}

void PWMData::CancelSpeedCallback(int32_t uid) {
  m_speedCallbacks = CancelCallback(m_speedCallbacks, uid);
}

void PWMData::InvokeSpeedCallback(HAL_Value value) {
  InvokeCallback(m_speedCallbacks, "Speed", &value);
}

double PWMData::GetSpeed() { return m_speed; }

void PWMData::SetSpeed(double speed) {
  double oldValue = m_speed.exchange(speed);
  if (oldValue != speed) {
    InvokeSpeedCallback(MakeDouble(speed));
  }
}

int32_t PWMData::RegisterPositionCallback(HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_positionCallbacks = RegisterCallback(m_positionCallbacks, "Position",
                                           callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetPosition());
    callback("Position", param, &value);
  }
  return newUid;
}

void PWMData::CancelPositionCallback(int32_t uid) {
  m_positionCallbacks = CancelCallback(m_positionCallbacks, uid);
}

void PWMData::InvokePositionCallback(HAL_Value value) {
  InvokeCallback(m_positionCallbacks, "Position", &value);
}

double PWMData::GetPosition() { return m_position; }

void PWMData::SetPosition(double position) {
  double oldValue = m_position.exchange(position);
  if (oldValue != position) {
    InvokePositionCallback(MakeDouble(position));
  }
}

int32_t PWMData::RegisterPeriodScaleCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_periodScaleCallbacks = RegisterCallback(
        m_periodScaleCallbacks, "PeriodScale", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetPeriodScale());
    callback("PeriodScale", param, &value);
  }
  return newUid;
}

void PWMData::CancelPeriodScaleCallback(int32_t uid) {
  m_periodScaleCallbacks = CancelCallback(m_periodScaleCallbacks, uid);
}

void PWMData::InvokePeriodScaleCallback(HAL_Value value) {
  InvokeCallback(m_periodScaleCallbacks, "PeriodScale", &value);
}

int32_t PWMData::GetPeriodScale() { return m_periodScale; }

void PWMData::SetPeriodScale(int32_t periodScale) {
  int32_t oldValue = m_periodScale.exchange(periodScale);
  if (oldValue != periodScale) {
    InvokePeriodScaleCallback(MakeInt(periodScale));
  }
}

int32_t PWMData::RegisterZeroLatchCallback(HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_zeroLatchCallbacks = RegisterCallback(m_zeroLatchCallbacks, "ZeroLatch",
                                            callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetZeroLatch());
    callback("ZeroLatch", param, &value);
  }
  return newUid;
}

void PWMData::CancelZeroLatchCallback(int32_t uid) {
  m_zeroLatchCallbacks = CancelCallback(m_zeroLatchCallbacks, uid);
}

void PWMData::InvokeZeroLatchCallback(HAL_Value value) {
  InvokeCallback(m_zeroLatchCallbacks, "ZeroLatch", &value);
}

HAL_Bool PWMData::GetZeroLatch() { return m_zeroLatch; }

void PWMData::SetZeroLatch(HAL_Bool zeroLatch) {
  HAL_Bool oldValue = m_zeroLatch.exchange(zeroLatch);
  if (oldValue != zeroLatch) {
    InvokeZeroLatchCallback(MakeBoolean(zeroLatch));
  }
}

extern "C" {
void HALSIM_ResetPWMData(int32_t index) { SimPWMData[index].ResetData(); }

int32_t HALSIM_RegisterPWMInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelPWMInitializedCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetPWMInitialized(int32_t index) {
  return SimPWMData[index].GetInitialized();
}

void HALSIM_SetPWMInitialized(int32_t index, HAL_Bool initialized) {
  SimPWMData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterPWMRawValueCallback(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterRawValueCallback(callback, param,
                                                    initialNotify);
}

void HALSIM_CancelPWMRawValueCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelRawValueCallback(uid);
}

int32_t HALSIM_GetPWMRawValue(int32_t index) {
  return SimPWMData[index].GetRawValue();
}

void HALSIM_SetPWMRawValue(int32_t index, int32_t rawValue) {
  SimPWMData[index].SetRawValue(rawValue);
}

int32_t HALSIM_RegisterPWMSpeedCallback(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterSpeedCallback(callback, param,
                                                 initialNotify);
}

void HALSIM_CancelPWMSpeedCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelSpeedCallback(uid);
}

double HALSIM_GetPWMSpeed(int32_t index) {
  return SimPWMData[index].GetSpeed();
}

void HALSIM_SetPWMSpeed(int32_t index, double speed) {
  SimPWMData[index].SetSpeed(speed);
}

int32_t HALSIM_RegisterPWMPositionCallback(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterPositionCallback(callback, param,
                                                    initialNotify);
}

void HALSIM_CancelPWMPositionCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelPositionCallback(uid);
}

double HALSIM_GetPWMPosition(int32_t index) {
  return SimPWMData[index].GetPosition();
}

void HALSIM_SetPWMPosition(int32_t index, double position) {
  SimPWMData[index].SetPosition(position);
}

int32_t HALSIM_RegisterPWMPeriodScaleCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterPeriodScaleCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelPWMPeriodScaleCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelPeriodScaleCallback(uid);
}

int32_t HALSIM_GetPWMPeriodScale(int32_t index) {
  return SimPWMData[index].GetPeriodScale();
}

void HALSIM_SetPWMPeriodScale(int32_t index, int32_t periodScale) {
  SimPWMData[index].SetPeriodScale(periodScale);
}

int32_t HALSIM_RegisterPWMZeroLatchCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimPWMData[index].RegisterZeroLatchCallback(callback, param,
                                                     initialNotify);
}

void HALSIM_CancelPWMZeroLatchCallback(int32_t index, int32_t uid) {
  SimPWMData[index].CancelZeroLatchCallback(uid);
}

HAL_Bool HALSIM_GetPWMZeroLatch(int32_t index) {
  return SimPWMData[index].GetZeroLatch();
}

void HALSIM_SetPWMZeroLatch(int32_t index, HAL_Bool zeroLatch) {
  SimPWMData[index].SetZeroLatch(zeroLatch);
}

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  SimPWMData[index].RegisterInitializedCallback(callback, param, initialNotify);
  SimPWMData[index].RegisterRawValueCallback(callback, param, initialNotify);
  SimPWMData[index].RegisterSpeedCallback(callback, param, initialNotify);
  SimPWMData[index].RegisterPositionCallback(callback, param, initialNotify);
  SimPWMData[index].RegisterPeriodScaleCallback(callback, param, initialNotify);
  SimPWMData[index].RegisterZeroLatchCallback(callback, param, initialNotify);
}
}  // extern "C"
