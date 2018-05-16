/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AccelerometerDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAccelerometerData() {
  static AccelerometerData sad[1];
  ::hal::SimAccelerometerData = sad;
}
}  // namespace init
}  // namespace hal

AccelerometerData* hal::SimAccelerometerData;
void AccelerometerData::ResetData() {
  m_active = false;
  m_activeCallbacks = nullptr;
  m_range = static_cast<HAL_AccelerometerRange>(0);
  m_rangeCallbacks = nullptr;
  m_x = 0.0;
  m_xCallbacks = nullptr;
  m_y = 0.0;
  m_yCallbacks = nullptr;
  m_z = 0.0;
  m_zCallbacks = nullptr;
}

int32_t AccelerometerData::RegisterActiveCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_activeCallbacks =
        RegisterCallback(m_activeCallbacks, "Active", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetActive());
    callback("Active", param, &value);
  }
  return newUid;
}

void AccelerometerData::CancelActiveCallback(int32_t uid) {
  m_activeCallbacks = CancelCallback(m_activeCallbacks, uid);
}

void AccelerometerData::InvokeActiveCallback(HAL_Value value) {
  InvokeCallback(m_activeCallbacks, "Active", &value);
}

HAL_Bool AccelerometerData::GetActive() { return m_active; }

void AccelerometerData::SetActive(HAL_Bool active) {
  HAL_Bool oldValue = m_active.exchange(active);
  if (oldValue != active) {
    InvokeActiveCallback(MakeBoolean(active));
  }
}

int32_t AccelerometerData::RegisterRangeCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_rangeCallbacks =
        RegisterCallback(m_rangeCallbacks, "Range", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeEnum(GetRange());
    callback("Range", param, &value);
  }
  return newUid;
}

void AccelerometerData::CancelRangeCallback(int32_t uid) {
  m_rangeCallbacks = CancelCallback(m_rangeCallbacks, uid);
}

void AccelerometerData::InvokeRangeCallback(HAL_Value value) {
  InvokeCallback(m_rangeCallbacks, "Range", &value);
}

HAL_AccelerometerRange AccelerometerData::GetRange() { return m_range; }

void AccelerometerData::SetRange(HAL_AccelerometerRange range) {
  HAL_AccelerometerRange oldValue = m_range.exchange(range);
  if (oldValue != range) {
    InvokeRangeCallback(MakeEnum(range));
  }
}

int32_t AccelerometerData::RegisterXCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_xCallbacks =
        RegisterCallback(m_xCallbacks, "X", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetX());
    callback("X", param, &value);
  }
  return newUid;
}

void AccelerometerData::CancelXCallback(int32_t uid) {
  m_xCallbacks = CancelCallback(m_xCallbacks, uid);
}

void AccelerometerData::InvokeXCallback(HAL_Value value) {
  InvokeCallback(m_xCallbacks, "X", &value);
}

double AccelerometerData::GetX() { return m_x; }

void AccelerometerData::SetX(double x) {
  double oldValue = m_x.exchange(x);
  if (oldValue != x) {
    InvokeXCallback(MakeDouble(x));
  }
}

int32_t AccelerometerData::RegisterYCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_yCallbacks =
        RegisterCallback(m_yCallbacks, "Y", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetY());
    callback("Y", param, &value);
  }
  return newUid;
}

void AccelerometerData::CancelYCallback(int32_t uid) {
  m_yCallbacks = CancelCallback(m_yCallbacks, uid);
}

void AccelerometerData::InvokeYCallback(HAL_Value value) {
  InvokeCallback(m_yCallbacks, "Y", &value);
}

double AccelerometerData::GetY() { return m_y; }

void AccelerometerData::SetY(double y) {
  double oldValue = m_y.exchange(y);
  if (oldValue != y) {
    InvokeYCallback(MakeDouble(y));
  }
}

int32_t AccelerometerData::RegisterZCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_zCallbacks =
        RegisterCallback(m_zCallbacks, "Z", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetZ());
    callback("Z", param, &value);
  }
  return newUid;
}

void AccelerometerData::CancelZCallback(int32_t uid) {
  m_zCallbacks = CancelCallback(m_zCallbacks, uid);
}

void AccelerometerData::InvokeZCallback(HAL_Value value) {
  InvokeCallback(m_zCallbacks, "Z", &value);
}

double AccelerometerData::GetZ() { return m_z; }

void AccelerometerData::SetZ(double z) {
  double oldValue = m_z.exchange(z);
  if (oldValue != z) {
    InvokeZCallback(MakeDouble(z));
  }
}

extern "C" {
void HALSIM_ResetAccelerometerData(int32_t index) {
  SimAccelerometerData[index].ResetData();
}

int32_t HALSIM_RegisterAccelerometerActiveCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimAccelerometerData[index].RegisterActiveCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelAccelerometerActiveCallback(int32_t index, int32_t uid) {
  SimAccelerometerData[index].CancelActiveCallback(uid);
}

HAL_Bool HALSIM_GetAccelerometerActive(int32_t index) {
  return SimAccelerometerData[index].GetActive();
}

void HALSIM_SetAccelerometerActive(int32_t index, HAL_Bool active) {
  SimAccelerometerData[index].SetActive(active);
}

int32_t HALSIM_RegisterAccelerometerRangeCallback(int32_t index,
                                                  HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  return SimAccelerometerData[index].RegisterRangeCallback(callback, param,
                                                           initialNotify);
}

void HALSIM_CancelAccelerometerRangeCallback(int32_t index, int32_t uid) {
  SimAccelerometerData[index].CancelRangeCallback(uid);
}

HAL_AccelerometerRange HALSIM_GetAccelerometerRange(int32_t index) {
  return SimAccelerometerData[index].GetRange();
}

void HALSIM_SetAccelerometerRange(int32_t index, HAL_AccelerometerRange range) {
  SimAccelerometerData[index].SetRange(range);
}

int32_t HALSIM_RegisterAccelerometerXCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimAccelerometerData[index].RegisterXCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelAccelerometerXCallback(int32_t index, int32_t uid) {
  SimAccelerometerData[index].CancelXCallback(uid);
}

double HALSIM_GetAccelerometerX(int32_t index) {
  return SimAccelerometerData[index].GetX();
}

void HALSIM_SetAccelerometerX(int32_t index, double x) {
  SimAccelerometerData[index].SetX(x);
}

int32_t HALSIM_RegisterAccelerometerYCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimAccelerometerData[index].RegisterYCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelAccelerometerYCallback(int32_t index, int32_t uid) {
  SimAccelerometerData[index].CancelYCallback(uid);
}

double HALSIM_GetAccelerometerY(int32_t index) {
  return SimAccelerometerData[index].GetY();
}

void HALSIM_SetAccelerometerY(int32_t index, double y) {
  SimAccelerometerData[index].SetY(y);
}

int32_t HALSIM_RegisterAccelerometerZCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimAccelerometerData[index].RegisterZCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelAccelerometerZCallback(int32_t index, int32_t uid) {
  SimAccelerometerData[index].CancelZCallback(uid);
}

double HALSIM_GetAccelerometerZ(int32_t index) {
  return SimAccelerometerData[index].GetZ();
}

void HALSIM_SetAccelerometerZ(int32_t index, double z) {
  SimAccelerometerData[index].SetZ(z);
}

void HALSIM_RegisterAccelerometerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  SimAccelerometerData[index].RegisterActiveCallback(callback, param,
                                                     initialNotify);
  SimAccelerometerData[index].RegisterRangeCallback(callback, param,
                                                    initialNotify);
  SimAccelerometerData[index].RegisterXCallback(callback, param, initialNotify);
  SimAccelerometerData[index].RegisterYCallback(callback, param, initialNotify);
  SimAccelerometerData[index].RegisterZCallback(callback, param, initialNotify);
}
}  // extern "C"
