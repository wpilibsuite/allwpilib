/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "SPIAccelerometerDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeSPIAccelerometerData() {
  static SPIAccelerometerData ssad[5];
  ::hal::SimSPIAccelerometerData = ssad;
}
}  // namespace init
}  // namespace hal

SPIAccelerometerData* hal::SimSPIAccelerometerData;
void SPIAccelerometerData::ResetData() {
  m_active = false;
  m_activeCallbacks = nullptr;
  m_range = 0;
  m_rangeCallbacks = nullptr;
  m_x = 0.0;
  m_xCallbacks = nullptr;
  m_y = 0.0;
  m_yCallbacks = nullptr;
  m_z = 0.0;
  m_zCallbacks = nullptr;
}

int32_t SPIAccelerometerData::RegisterActiveCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
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

void SPIAccelerometerData::CancelActiveCallback(int32_t uid) {
  m_activeCallbacks = CancelCallback(m_activeCallbacks, uid);
}

void SPIAccelerometerData::InvokeActiveCallback(HAL_Value value) {
  InvokeCallback(m_activeCallbacks, "Active", &value);
}

HAL_Bool SPIAccelerometerData::GetActive() { return m_active; }

void SPIAccelerometerData::SetActive(HAL_Bool active) {
  HAL_Bool oldValue = m_active.exchange(active);
  if (oldValue != active) {
    InvokeActiveCallback(MakeBoolean(active));
  }
}

int32_t SPIAccelerometerData::RegisterRangeCallback(HAL_NotifyCallback callback,
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
    HAL_Value value = MakeInt(GetRange());
    callback("Range", param, &value);
  }
  return newUid;
}

void SPIAccelerometerData::CancelRangeCallback(int32_t uid) {
  m_rangeCallbacks = CancelCallback(m_rangeCallbacks, uid);
}

void SPIAccelerometerData::InvokeRangeCallback(HAL_Value value) {
  InvokeCallback(m_rangeCallbacks, "Range", &value);
}

int32_t SPIAccelerometerData::GetRange() { return m_range; }

void SPIAccelerometerData::SetRange(int32_t range) {
  int32_t oldValue = m_range.exchange(range);
  if (oldValue != range) {
    InvokeRangeCallback(MakeInt(range));
  }
}

int32_t SPIAccelerometerData::RegisterXCallback(HAL_NotifyCallback callback,
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

void SPIAccelerometerData::CancelXCallback(int32_t uid) {
  m_xCallbacks = CancelCallback(m_xCallbacks, uid);
}

void SPIAccelerometerData::InvokeXCallback(HAL_Value value) {
  InvokeCallback(m_xCallbacks, "X", &value);
}

double SPIAccelerometerData::GetX() { return m_x; }

void SPIAccelerometerData::SetX(double x) {
  double oldValue = m_x.exchange(x);
  if (oldValue != x) {
    InvokeXCallback(MakeDouble(x));
  }
}

int32_t SPIAccelerometerData::RegisterYCallback(HAL_NotifyCallback callback,
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

void SPIAccelerometerData::CancelYCallback(int32_t uid) {
  m_yCallbacks = CancelCallback(m_yCallbacks, uid);
}

void SPIAccelerometerData::InvokeYCallback(HAL_Value value) {
  InvokeCallback(m_yCallbacks, "Y", &value);
}

double SPIAccelerometerData::GetY() { return m_y; }

void SPIAccelerometerData::SetY(double y) {
  double oldValue = m_y.exchange(y);
  if (oldValue != y) {
    InvokeYCallback(MakeDouble(y));
  }
}

int32_t SPIAccelerometerData::RegisterZCallback(HAL_NotifyCallback callback,
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

void SPIAccelerometerData::CancelZCallback(int32_t uid) {
  m_zCallbacks = CancelCallback(m_zCallbacks, uid);
}

void SPIAccelerometerData::InvokeZCallback(HAL_Value value) {
  InvokeCallback(m_zCallbacks, "Z", &value);
}

double SPIAccelerometerData::GetZ() { return m_z; }

void SPIAccelerometerData::SetZ(double z) {
  double oldValue = m_z.exchange(z);
  if (oldValue != z) {
    InvokeZCallback(MakeDouble(z));
  }
}

extern "C" {
void HALSIM_ResetSPIAccelerometerData(int32_t index) {
  SimSPIAccelerometerData[index].ResetData();
}

int32_t HALSIM_RegisterSPIAccelerometerActiveCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimSPIAccelerometerData[index].RegisterActiveCallback(callback, param,
                                                               initialNotify);
}

void HALSIM_CancelSPIAccelerometerActiveCallback(int32_t index, int32_t uid) {
  SimSPIAccelerometerData[index].CancelActiveCallback(uid);
}

HAL_Bool HALSIM_GetSPIAccelerometerActive(int32_t index) {
  return SimSPIAccelerometerData[index].GetActive();
}

void HALSIM_SetSPIAccelerometerActive(int32_t index, HAL_Bool active) {
  SimSPIAccelerometerData[index].SetActive(active);
}

int32_t HALSIM_RegisterSPIAccelerometerRangeCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimSPIAccelerometerData[index].RegisterRangeCallback(callback, param,
                                                              initialNotify);
}

void HALSIM_CancelSPIAccelerometerRangeCallback(int32_t index, int32_t uid) {
  SimSPIAccelerometerData[index].CancelRangeCallback(uid);
}

int32_t HALSIM_GetSPIAccelerometerRange(int32_t index) {
  return SimSPIAccelerometerData[index].GetRange();
}

void HALSIM_SetSPIAccelerometerRange(int32_t index, int32_t range) {
  SimSPIAccelerometerData[index].SetRange(range);
}

int32_t HALSIM_RegisterSPIAccelerometerXCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimSPIAccelerometerData[index].RegisterXCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelSPIAccelerometerXCallback(int32_t index, int32_t uid) {
  SimSPIAccelerometerData[index].CancelXCallback(uid);
}

double HALSIM_GetSPIAccelerometerX(int32_t index) {
  return SimSPIAccelerometerData[index].GetX();
}

void HALSIM_SetSPIAccelerometerX(int32_t index, double x) {
  SimSPIAccelerometerData[index].SetX(x);
}

int32_t HALSIM_RegisterSPIAccelerometerYCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimSPIAccelerometerData[index].RegisterYCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelSPIAccelerometerYCallback(int32_t index, int32_t uid) {
  SimSPIAccelerometerData[index].CancelYCallback(uid);
}

double HALSIM_GetSPIAccelerometerY(int32_t index) {
  return SimSPIAccelerometerData[index].GetY();
}

void HALSIM_SetSPIAccelerometerY(int32_t index, double y) {
  SimSPIAccelerometerData[index].SetY(y);
}

int32_t HALSIM_RegisterSPIAccelerometerZCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimSPIAccelerometerData[index].RegisterZCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelSPIAccelerometerZCallback(int32_t index, int32_t uid) {
  SimSPIAccelerometerData[index].CancelZCallback(uid);
}

double HALSIM_GetSPIAccelerometerZ(int32_t index) {
  return SimSPIAccelerometerData[index].GetZ();
}

void HALSIM_SetSPIAccelerometerZ(int32_t index, double z) {
  SimSPIAccelerometerData[index].SetZ(z);
}

void HALSIM_RegisterSPIAccelerometerAllCallbcaks(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  SimSPIAccelerometerData[index].RegisterActiveCallback(callback, param,
                                                        initialNotify);
  SimSPIAccelerometerData[index].RegisterRangeCallback(callback, param,
                                                       initialNotify);
  SimSPIAccelerometerData[index].RegisterXCallback(callback, param,
                                                   initialNotify);
  SimSPIAccelerometerData[index].RegisterYCallback(callback, param,
                                                   initialNotify);
  SimSPIAccelerometerData[index].RegisterZCallback(callback, param,
                                                   initialNotify);
}
}  // extern "C"
