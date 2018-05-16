/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "DIODataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeDIOData() {
  static DIOData sdd[kNumDigitalChannels];
  ::hal::SimDIOData = sdd;
}
}  // namespace init
}  // namespace hal

DIOData* hal::SimDIOData;
void DIOData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_value = true;
  m_valueCallbacks = nullptr;
  m_pulseLength = 0.0;
  m_pulseLengthCallbacks = nullptr;
  m_isInput = true;
  m_isInputCallbacks = nullptr;
  m_filterIndex = -1;
  m_filterIndexCallbacks = nullptr;
}

int32_t DIOData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void DIOData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void DIOData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool DIOData::GetInitialized() { return m_initialized; }

void DIOData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t DIOData::RegisterValueCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_valueCallbacks =
        RegisterCallback(m_valueCallbacks, "Value", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetValue());
    callback("Value", param, &value);
  }
  return newUid;
}

void DIOData::CancelValueCallback(int32_t uid) {
  m_valueCallbacks = CancelCallback(m_valueCallbacks, uid);
}

void DIOData::InvokeValueCallback(HAL_Value value) {
  InvokeCallback(m_valueCallbacks, "Value", &value);
}

HAL_Bool DIOData::GetValue() { return m_value; }

void DIOData::SetValue(HAL_Bool value) {
  HAL_Bool oldValue = m_value.exchange(value);
  if (oldValue != value) {
    InvokeValueCallback(MakeBoolean(value));
  }
}

int32_t DIOData::RegisterPulseLengthCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_pulseLengthCallbacks = RegisterCallback(
        m_pulseLengthCallbacks, "PulseLength", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetPulseLength());
    callback("PulseLength", param, &value);
  }
  return newUid;
}

void DIOData::CancelPulseLengthCallback(int32_t uid) {
  m_pulseLengthCallbacks = CancelCallback(m_pulseLengthCallbacks, uid);
}

void DIOData::InvokePulseLengthCallback(HAL_Value value) {
  InvokeCallback(m_pulseLengthCallbacks, "PulseLength", &value);
}

double DIOData::GetPulseLength() { return m_pulseLength; }

void DIOData::SetPulseLength(double pulseLength) {
  double oldValue = m_pulseLength.exchange(pulseLength);
  if (oldValue != pulseLength) {
    InvokePulseLengthCallback(MakeDouble(pulseLength));
  }
}

int32_t DIOData::RegisterIsInputCallback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_isInputCallbacks = RegisterCallback(m_isInputCallbacks, "IsInput",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetIsInput());
    callback("IsInput", param, &value);
  }
  return newUid;
}

void DIOData::CancelIsInputCallback(int32_t uid) {
  m_isInputCallbacks = CancelCallback(m_isInputCallbacks, uid);
}

void DIOData::InvokeIsInputCallback(HAL_Value value) {
  InvokeCallback(m_isInputCallbacks, "IsInput", &value);
}

HAL_Bool DIOData::GetIsInput() { return m_isInput; }

void DIOData::SetIsInput(HAL_Bool isInput) {
  HAL_Bool oldValue = m_isInput.exchange(isInput);
  if (oldValue != isInput) {
    InvokeIsInputCallback(MakeBoolean(isInput));
  }
}

int32_t DIOData::RegisterFilterIndexCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_filterIndexCallbacks = RegisterCallback(
        m_filterIndexCallbacks, "FilterIndex", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetFilterIndex());
    callback("FilterIndex", param, &value);
  }
  return newUid;
}

void DIOData::CancelFilterIndexCallback(int32_t uid) {
  m_filterIndexCallbacks = CancelCallback(m_filterIndexCallbacks, uid);
}

void DIOData::InvokeFilterIndexCallback(HAL_Value value) {
  InvokeCallback(m_filterIndexCallbacks, "FilterIndex", &value);
}

int32_t DIOData::GetFilterIndex() { return m_filterIndex; }

void DIOData::SetFilterIndex(int32_t filterIndex) {
  int32_t oldValue = m_filterIndex.exchange(filterIndex);
  if (oldValue != filterIndex) {
    InvokeFilterIndexCallback(MakeInt(filterIndex));
  }
}

extern "C" {
void HALSIM_ResetDIOData(int32_t index) { SimDIOData[index].ResetData(); }

int32_t HALSIM_RegisterDIOInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimDIOData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelDIOInitializedCallback(int32_t index, int32_t uid) {
  SimDIOData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetDIOInitialized(int32_t index) {
  return SimDIOData[index].GetInitialized();
}

void HALSIM_SetDIOInitialized(int32_t index, HAL_Bool initialized) {
  SimDIOData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterDIOValueCallback(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  return SimDIOData[index].RegisterValueCallback(callback, param,
                                                 initialNotify);
}

void HALSIM_CancelDIOValueCallback(int32_t index, int32_t uid) {
  SimDIOData[index].CancelValueCallback(uid);
}

HAL_Bool HALSIM_GetDIOValue(int32_t index) {
  return SimDIOData[index].GetValue();
}

void HALSIM_SetDIOValue(int32_t index, HAL_Bool value) {
  SimDIOData[index].SetValue(value);
}

int32_t HALSIM_RegisterDIOPulseLengthCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimDIOData[index].RegisterPulseLengthCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelDIOPulseLengthCallback(int32_t index, int32_t uid) {
  SimDIOData[index].CancelPulseLengthCallback(uid);
}

double HALSIM_GetDIOPulseLength(int32_t index) {
  return SimDIOData[index].GetPulseLength();
}

void HALSIM_SetDIOPulseLength(int32_t index, double pulseLength) {
  SimDIOData[index].SetPulseLength(pulseLength);
}

int32_t HALSIM_RegisterDIOIsInputCallback(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  return SimDIOData[index].RegisterIsInputCallback(callback, param,
                                                   initialNotify);
}

void HALSIM_CancelDIOIsInputCallback(int32_t index, int32_t uid) {
  SimDIOData[index].CancelIsInputCallback(uid);
}

HAL_Bool HALSIM_GetDIOIsInput(int32_t index) {
  return SimDIOData[index].GetIsInput();
}

void HALSIM_SetDIOIsInput(int32_t index, HAL_Bool isInput) {
  SimDIOData[index].SetIsInput(isInput);
}

int32_t HALSIM_RegisterDIOFilterIndexCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimDIOData[index].RegisterFilterIndexCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelDIOFilterIndexCallback(int32_t index, int32_t uid) {
  SimDIOData[index].CancelFilterIndexCallback(uid);
}

int32_t HALSIM_GetDIOFilterIndex(int32_t index) {
  return SimDIOData[index].GetFilterIndex();
}

void HALSIM_SetDIOFilterIndex(int32_t index, int32_t filterIndex) {
  SimDIOData[index].SetFilterIndex(filterIndex);
}

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  SimDIOData[index].RegisterInitializedCallback(callback, param, initialNotify);
  SimDIOData[index].RegisterValueCallback(callback, param, initialNotify);
  SimDIOData[index].RegisterPulseLengthCallback(callback, param, initialNotify);
  SimDIOData[index].RegisterIsInputCallback(callback, param, initialNotify);
  SimDIOData[index].RegisterFilterIndexCallback(callback, param, initialNotify);
}
}  // extern "C"
