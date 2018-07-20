/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ThreeAxisAccelerometerData.h"

#include <mockdata/NotifyCallbackHelpers.h>

using namespace hal;

ThreeAxisAccelerometerData::ThreeAxisAccelerometerData() {}

ThreeAxisAccelerometerData::~ThreeAxisAccelerometerData() {}
void ThreeAxisAccelerometerData::ResetData() {
  m_x = 0.0;
  m_xCallbacks = nullptr;
  m_y = 0.0;
  m_yCallbacks = nullptr;
  m_z = 0.0;
  m_zCallbacks = nullptr;
}

int32_t ThreeAxisAccelerometerData::RegisterXCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
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

void ThreeAxisAccelerometerData::CancelXCallback(int32_t uid) {
  m_xCallbacks = CancelCallback(m_xCallbacks, uid);
}

void ThreeAxisAccelerometerData::InvokeXCallback(HAL_Value value) {
  InvokeCallback(m_xCallbacks, "X", &value);
}

double ThreeAxisAccelerometerData::GetX() { return m_x; }

void ThreeAxisAccelerometerData::SetX(double x) {
  double oldValue = m_x.exchange(x);
  if (oldValue != x) {
    InvokeXCallback(MakeDouble(x));
  }
}

int32_t ThreeAxisAccelerometerData::RegisterYCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
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

void ThreeAxisAccelerometerData::CancelYCallback(int32_t uid) {
  m_yCallbacks = CancelCallback(m_yCallbacks, uid);
}

void ThreeAxisAccelerometerData::InvokeYCallback(HAL_Value value) {
  InvokeCallback(m_yCallbacks, "Y", &value);
}

double ThreeAxisAccelerometerData::GetY() { return m_y; }

void ThreeAxisAccelerometerData::SetY(double y) {
  double oldValue = m_y.exchange(y);
  if (oldValue != y) {
    InvokeYCallback(MakeDouble(y));
  }
}

int32_t ThreeAxisAccelerometerData::RegisterZCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
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

void ThreeAxisAccelerometerData::CancelZCallback(int32_t uid) {
  m_zCallbacks = CancelCallback(m_zCallbacks, uid);
}

void ThreeAxisAccelerometerData::InvokeZCallback(HAL_Value value) {
  InvokeCallback(m_zCallbacks, "Z", &value);
}

double ThreeAxisAccelerometerData::GetZ() { return m_z; }

void ThreeAxisAccelerometerData::SetZ(double z) {
  double oldValue = m_z.exchange(z);
  if (oldValue != z) {
    InvokeZCallback(MakeDouble(z));
  }
}
