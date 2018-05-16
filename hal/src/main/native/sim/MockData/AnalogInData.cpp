/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AnalogInDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAnalogInData() {
  static AnalogInData sind[kNumAnalogInputs];
  ::hal::SimAnalogInData = sind;
}
}  // namespace init
}  // namespace hal

AnalogInData* hal::SimAnalogInData;
void AnalogInData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_averageBits = 7;
  m_averageBitsCallbacks = nullptr;
  m_oversampleBits = 0;
  m_oversampleBitsCallbacks = nullptr;
  m_voltage = 0.0;
  m_voltageCallbacks = nullptr;
  m_accumulatorInitialized = false;
  m_accumulatorInitializedCallbacks = nullptr;
  m_accumulatorValue = 0;
  m_accumulatorValueCallbacks = nullptr;
  m_accumulatorCount = 0;
  m_accumulatorCountCallbacks = nullptr;
  m_accumulatorCenter = 0;
  m_accumulatorCenterCallbacks = nullptr;
  m_accumulatorDeadband = 0;
  m_accumulatorDeadbandCallbacks = nullptr;
}

int32_t AnalogInData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void AnalogInData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void AnalogInData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool AnalogInData::GetInitialized() { return m_initialized; }

void AnalogInData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t AnalogInData::RegisterAverageBitsCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_averageBitsCallbacks = RegisterCallback(
        m_averageBitsCallbacks, "AverageBits", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetAverageBits());
    callback("AverageBits", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAverageBitsCallback(int32_t uid) {
  m_averageBitsCallbacks = CancelCallback(m_averageBitsCallbacks, uid);
}

void AnalogInData::InvokeAverageBitsCallback(HAL_Value value) {
  InvokeCallback(m_averageBitsCallbacks, "AverageBits", &value);
}

int32_t AnalogInData::GetAverageBits() { return m_averageBits; }

void AnalogInData::SetAverageBits(int32_t averageBits) {
  int32_t oldValue = m_averageBits.exchange(averageBits);
  if (oldValue != averageBits) {
    InvokeAverageBitsCallback(MakeInt(averageBits));
  }
}

int32_t AnalogInData::RegisterOversampleBitsCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_oversampleBitsCallbacks = RegisterCallback(
        m_oversampleBitsCallbacks, "OversampleBits", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetOversampleBits());
    callback("OversampleBits", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelOversampleBitsCallback(int32_t uid) {
  m_oversampleBitsCallbacks = CancelCallback(m_oversampleBitsCallbacks, uid);
}

void AnalogInData::InvokeOversampleBitsCallback(HAL_Value value) {
  InvokeCallback(m_oversampleBitsCallbacks, "OversampleBits", &value);
}

int32_t AnalogInData::GetOversampleBits() { return m_oversampleBits; }

void AnalogInData::SetOversampleBits(int32_t oversampleBits) {
  int32_t oldValue = m_oversampleBits.exchange(oversampleBits);
  if (oldValue != oversampleBits) {
    InvokeOversampleBitsCallback(MakeInt(oversampleBits));
  }
}

int32_t AnalogInData::RegisterVoltageCallback(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_voltageCallbacks = RegisterCallback(m_voltageCallbacks, "Voltage",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetVoltage());
    callback("Voltage", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelVoltageCallback(int32_t uid) {
  m_voltageCallbacks = CancelCallback(m_voltageCallbacks, uid);
}

void AnalogInData::InvokeVoltageCallback(HAL_Value value) {
  InvokeCallback(m_voltageCallbacks, "Voltage", &value);
}

double AnalogInData::GetVoltage() { return m_voltage; }

void AnalogInData::SetVoltage(double voltage) {
  double oldValue = m_voltage.exchange(voltage);
  if (oldValue != voltage) {
    InvokeVoltageCallback(MakeDouble(voltage));
  }
}

int32_t AnalogInData::RegisterAccumulatorInitializedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_accumulatorInitializedCallbacks =
        RegisterCallback(m_accumulatorInitializedCallbacks,
                         "AccumulatorInitialized", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetAccumulatorInitialized());
    callback("AccumulatorInitialized", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAccumulatorInitializedCallback(int32_t uid) {
  m_accumulatorInitializedCallbacks =
      CancelCallback(m_accumulatorInitializedCallbacks, uid);
}

void AnalogInData::InvokeAccumulatorInitializedCallback(HAL_Value value) {
  InvokeCallback(m_accumulatorInitializedCallbacks, "AccumulatorInitialized",
                 &value);
}

HAL_Bool AnalogInData::GetAccumulatorInitialized() {
  return m_accumulatorInitialized;
}

void AnalogInData::SetAccumulatorInitialized(HAL_Bool accumulatorInitialized) {
  HAL_Bool oldValue = m_accumulatorInitialized.exchange(accumulatorInitialized);
  if (oldValue != accumulatorInitialized) {
    InvokeAccumulatorInitializedCallback(MakeBoolean(accumulatorInitialized));
  }
}

int32_t AnalogInData::RegisterAccumulatorValueCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_accumulatorValueCallbacks =
        RegisterCallback(m_accumulatorValueCallbacks, "AccumulatorValue",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeLong(GetAccumulatorValue());
    callback("AccumulatorValue", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAccumulatorValueCallback(int32_t uid) {
  m_accumulatorValueCallbacks =
      CancelCallback(m_accumulatorValueCallbacks, uid);
}

void AnalogInData::InvokeAccumulatorValueCallback(HAL_Value value) {
  InvokeCallback(m_accumulatorValueCallbacks, "AccumulatorValue", &value);
}

int64_t AnalogInData::GetAccumulatorValue() { return m_accumulatorValue; }

void AnalogInData::SetAccumulatorValue(int64_t accumulatorValue) {
  int64_t oldValue = m_accumulatorValue.exchange(accumulatorValue);
  if (oldValue != accumulatorValue) {
    InvokeAccumulatorValueCallback(MakeLong(accumulatorValue));
  }
}

int32_t AnalogInData::RegisterAccumulatorCountCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_accumulatorCountCallbacks =
        RegisterCallback(m_accumulatorCountCallbacks, "AccumulatorCount",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeLong(GetAccumulatorCount());
    callback("AccumulatorCount", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAccumulatorCountCallback(int32_t uid) {
  m_accumulatorCountCallbacks =
      CancelCallback(m_accumulatorCountCallbacks, uid);
}

void AnalogInData::InvokeAccumulatorCountCallback(HAL_Value value) {
  InvokeCallback(m_accumulatorCountCallbacks, "AccumulatorCount", &value);
}

int64_t AnalogInData::GetAccumulatorCount() { return m_accumulatorCount; }

void AnalogInData::SetAccumulatorCount(int64_t accumulatorCount) {
  int64_t oldValue = m_accumulatorCount.exchange(accumulatorCount);
  if (oldValue != accumulatorCount) {
    InvokeAccumulatorCountCallback(MakeLong(accumulatorCount));
  }
}

int32_t AnalogInData::RegisterAccumulatorCenterCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_accumulatorCenterCallbacks =
        RegisterCallback(m_accumulatorCenterCallbacks, "AccumulatorCenter",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetAccumulatorCenter());
    callback("AccumulatorCenter", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAccumulatorCenterCallback(int32_t uid) {
  m_accumulatorCenterCallbacks =
      CancelCallback(m_accumulatorCenterCallbacks, uid);
}

void AnalogInData::InvokeAccumulatorCenterCallback(HAL_Value value) {
  InvokeCallback(m_accumulatorCenterCallbacks, "AccumulatorCenter", &value);
}

int32_t AnalogInData::GetAccumulatorCenter() { return m_accumulatorCenter; }

void AnalogInData::SetAccumulatorCenter(int32_t accumulatorCenter) {
  int32_t oldValue = m_accumulatorCenter.exchange(accumulatorCenter);
  if (oldValue != accumulatorCenter) {
    InvokeAccumulatorCenterCallback(MakeInt(accumulatorCenter));
  }
}

int32_t AnalogInData::RegisterAccumulatorDeadbandCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_accumulatorDeadbandCallbacks =
        RegisterCallback(m_accumulatorDeadbandCallbacks, "AccumulatorDeadband",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetAccumulatorDeadband());
    callback("AccumulatorDeadband", param, &value);
  }
  return newUid;
}

void AnalogInData::CancelAccumulatorDeadbandCallback(int32_t uid) {
  m_accumulatorDeadbandCallbacks =
      CancelCallback(m_accumulatorDeadbandCallbacks, uid);
}

void AnalogInData::InvokeAccumulatorDeadbandCallback(HAL_Value value) {
  InvokeCallback(m_accumulatorDeadbandCallbacks, "AccumulatorDeadband", &value);
}

int32_t AnalogInData::GetAccumulatorDeadband() { return m_accumulatorDeadband; }

void AnalogInData::SetAccumulatorDeadband(int32_t accumulatorDeadband) {
  int32_t oldValue = m_accumulatorDeadband.exchange(accumulatorDeadband);
  if (oldValue != accumulatorDeadband) {
    InvokeAccumulatorDeadbandCallback(MakeInt(accumulatorDeadband));
  }
}

extern "C" {
void HALSIM_ResetAnalogInData(int32_t index) {
  SimAnalogInData[index].ResetData();
}

int32_t HALSIM_RegisterAnalogInInitializedCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterInitializedCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelAnalogInInitializedCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetAnalogInInitialized(int32_t index) {
  return SimAnalogInData[index].GetInitialized();
}

void HALSIM_SetAnalogInInitialized(int32_t index, HAL_Bool initialized) {
  SimAnalogInData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterAnalogInAverageBitsCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAverageBitsCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelAnalogInAverageBitsCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelAverageBitsCallback(uid);
}

int32_t HALSIM_GetAnalogInAverageBits(int32_t index) {
  return SimAnalogInData[index].GetAverageBits();
}

void HALSIM_SetAnalogInAverageBits(int32_t index, int32_t averageBits) {
  SimAnalogInData[index].SetAverageBits(averageBits);
}

int32_t HALSIM_RegisterAnalogInOversampleBitsCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterOversampleBitsCallback(callback, param,
                                                               initialNotify);
}

void HALSIM_CancelAnalogInOversampleBitsCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelOversampleBitsCallback(uid);
}

int32_t HALSIM_GetAnalogInOversampleBits(int32_t index) {
  return SimAnalogInData[index].GetOversampleBits();
}

void HALSIM_SetAnalogInOversampleBits(int32_t index, int32_t oversampleBits) {
  SimAnalogInData[index].SetOversampleBits(oversampleBits);
}

int32_t HALSIM_RegisterAnalogInVoltageCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterVoltageCallback(callback, param,
                                                        initialNotify);
}

void HALSIM_CancelAnalogInVoltageCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelVoltageCallback(uid);
}

double HALSIM_GetAnalogInVoltage(int32_t index) {
  return SimAnalogInData[index].GetVoltage();
}

void HALSIM_SetAnalogInVoltage(int32_t index, double voltage) {
  SimAnalogInData[index].SetVoltage(voltage);
}

int32_t HALSIM_RegisterAnalogInAccumulatorInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAccumulatorInitializedCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogInAccumulatorInitializedCallback(int32_t index,
                                                         int32_t uid) {
  SimAnalogInData[index].CancelAccumulatorInitializedCallback(uid);
}

HAL_Bool HALSIM_GetAnalogInAccumulatorInitialized(int32_t index) {
  return SimAnalogInData[index].GetAccumulatorInitialized();
}

void HALSIM_SetAnalogInAccumulatorInitialized(int32_t index,
                                              HAL_Bool accumulatorInitialized) {
  SimAnalogInData[index].SetAccumulatorInitialized(accumulatorInitialized);
}

int32_t HALSIM_RegisterAnalogInAccumulatorValueCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAccumulatorValueCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogInAccumulatorValueCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelAccumulatorValueCallback(uid);
}

int64_t HALSIM_GetAnalogInAccumulatorValue(int32_t index) {
  return SimAnalogInData[index].GetAccumulatorValue();
}

void HALSIM_SetAnalogInAccumulatorValue(int32_t index,
                                        int64_t accumulatorValue) {
  SimAnalogInData[index].SetAccumulatorValue(accumulatorValue);
}

int32_t HALSIM_RegisterAnalogInAccumulatorCountCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAccumulatorCountCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogInAccumulatorCountCallback(int32_t index, int32_t uid) {
  SimAnalogInData[index].CancelAccumulatorCountCallback(uid);
}

int64_t HALSIM_GetAnalogInAccumulatorCount(int32_t index) {
  return SimAnalogInData[index].GetAccumulatorCount();
}

void HALSIM_SetAnalogInAccumulatorCount(int32_t index,
                                        int64_t accumulatorCount) {
  SimAnalogInData[index].SetAccumulatorCount(accumulatorCount);
}

int32_t HALSIM_RegisterAnalogInAccumulatorCenterCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAccumulatorCenterCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogInAccumulatorCenterCallback(int32_t index,
                                                    int32_t uid) {
  SimAnalogInData[index].CancelAccumulatorCenterCallback(uid);
}

int32_t HALSIM_GetAnalogInAccumulatorCenter(int32_t index) {
  return SimAnalogInData[index].GetAccumulatorCenter();
}

void HALSIM_SetAnalogInAccumulatorCenter(int32_t index,
                                         int32_t accumulatorCenter) {
  SimAnalogInData[index].SetAccumulatorCenter(accumulatorCenter);
}

int32_t HALSIM_RegisterAnalogInAccumulatorDeadbandCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimAnalogInData[index].RegisterAccumulatorDeadbandCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelAnalogInAccumulatorDeadbandCallback(int32_t index,
                                                      int32_t uid) {
  SimAnalogInData[index].CancelAccumulatorDeadbandCallback(uid);
}

int32_t HALSIM_GetAnalogInAccumulatorDeadband(int32_t index) {
  return SimAnalogInData[index].GetAccumulatorDeadband();
}

void HALSIM_SetAnalogInAccumulatorDeadband(int32_t index,
                                           int32_t accumulatorDeadband) {
  SimAnalogInData[index].SetAccumulatorDeadband(accumulatorDeadband);
}

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  SimAnalogInData[index].RegisterInitializedCallback(callback, param,
                                                     initialNotify);
  SimAnalogInData[index].RegisterAverageBitsCallback(callback, param,
                                                     initialNotify);
  SimAnalogInData[index].RegisterOversampleBitsCallback(callback, param,
                                                        initialNotify);
  SimAnalogInData[index].RegisterVoltageCallback(callback, param,
                                                 initialNotify);
  SimAnalogInData[index].RegisterAccumulatorInitializedCallback(callback, param,
                                                                initialNotify);
  SimAnalogInData[index].RegisterAccumulatorValueCallback(callback, param,
                                                          initialNotify);
  SimAnalogInData[index].RegisterAccumulatorCountCallback(callback, param,
                                                          initialNotify);
  SimAnalogInData[index].RegisterAccumulatorCenterCallback(callback, param,
                                                           initialNotify);
  SimAnalogInData[index].RegisterAccumulatorDeadbandCallback(callback, param,
                                                             initialNotify);
}
}  // extern "C"
