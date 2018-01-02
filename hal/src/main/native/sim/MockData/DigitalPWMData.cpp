/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "DigitalPWMDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeDigitalPWMData() {
  static DigitalPWMData sdpd[kNumDigitalPWMOutputs];
  ::hal::SimDigitalPWMData = sdpd;
}
}  // namespace init
}  // namespace hal

DigitalPWMData* hal::SimDigitalPWMData;
void DigitalPWMData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_dutyCycle = false;
  m_dutyCycleCallbacks = nullptr;
  m_pin = 0;
  m_pinCallbacks = nullptr;
}

int32_t DigitalPWMData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void DigitalPWMData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void DigitalPWMData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool DigitalPWMData::GetInitialized() { return m_initialized; }

void DigitalPWMData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t DigitalPWMData::RegisterDutyCycleCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_dutyCycleCallbacks = RegisterCallback(m_dutyCycleCallbacks, "DutyCycle",
                                            callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetDutyCycle());
    callback("DutyCycle", param, &value);
  }
  return newUid;
}

void DigitalPWMData::CancelDutyCycleCallback(int32_t uid) {
  m_dutyCycleCallbacks = CancelCallback(m_dutyCycleCallbacks, uid);
}

void DigitalPWMData::InvokeDutyCycleCallback(HAL_Value value) {
  InvokeCallback(m_dutyCycleCallbacks, "DutyCycle", &value);
}

double DigitalPWMData::GetDutyCycle() { return m_dutyCycle; }

void DigitalPWMData::SetDutyCycle(double dutyCycle) {
  double oldValue = m_dutyCycle.exchange(dutyCycle);
  if (oldValue != dutyCycle) {
    InvokeDutyCycleCallback(MakeDouble(dutyCycle));
  }
}

int32_t DigitalPWMData::RegisterPinCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_pinCallbacks =
        RegisterCallback(m_pinCallbacks, "Pin", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetPin());
    callback("Pin", param, &value);
  }
  return newUid;
}

void DigitalPWMData::CancelPinCallback(int32_t uid) {
  m_pinCallbacks = CancelCallback(m_pinCallbacks, uid);
}

void DigitalPWMData::InvokePinCallback(HAL_Value value) {
  InvokeCallback(m_pinCallbacks, "Pin", &value);
}

int32_t DigitalPWMData::GetPin() { return m_pin; }

void DigitalPWMData::SetPin(int32_t pin) {
  int32_t oldValue = m_pin.exchange(pin);
  if (oldValue != pin) {
    InvokePinCallback(MakeInt(pin));
  }
}

extern "C" {
void HALSIM_ResetDigitalPWMData(int32_t index) {
  SimDigitalPWMData[index].ResetData();
}

int32_t HALSIM_RegisterDigitalPWMInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimDigitalPWMData[index].RegisterInitializedCallback(callback, param,
                                                              initialNotify);
}

void HALSIM_CancelDigitalPWMInitializedCallback(int32_t index, int32_t uid) {
  SimDigitalPWMData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetDigitalPWMInitialized(int32_t index) {
  return SimDigitalPWMData[index].GetInitialized();
}

void HALSIM_SetDigitalPWMInitialized(int32_t index, HAL_Bool initialized) {
  SimDigitalPWMData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterDigitalPWMDutyCycleCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimDigitalPWMData[index].RegisterDutyCycleCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelDigitalPWMDutyCycleCallback(int32_t index, int32_t uid) {
  SimDigitalPWMData[index].CancelDutyCycleCallback(uid);
}

double HALSIM_GetDigitalPWMDutyCycle(int32_t index) {
  return SimDigitalPWMData[index].GetDutyCycle();
}

void HALSIM_SetDigitalPWMDutyCycle(int32_t index, double dutyCycle) {
  SimDigitalPWMData[index].SetDutyCycle(dutyCycle);
}

int32_t HALSIM_RegisterDigitalPWMPinCallback(int32_t index,
                                             HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  return SimDigitalPWMData[index].RegisterPinCallback(callback, param,
                                                      initialNotify);
}

void HALSIM_CancelDigitalPWMPinCallback(int32_t index, int32_t uid) {
  SimDigitalPWMData[index].CancelPinCallback(uid);
}

int32_t HALSIM_GetDigitalPWMPin(int32_t index) {
  return SimDigitalPWMData[index].GetPin();
}

void HALSIM_SetDigitalPWMPin(int32_t index, int32_t pin) {
  SimDigitalPWMData[index].SetPin(pin);
}

void HALSIM_RegisterDigitalPWMAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  SimDigitalPWMData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
  SimDigitalPWMData[index].RegisterDutyCycleCallback(callback, param,
                                                     initialNotify);
  SimDigitalPWMData[index].RegisterPinCallback(callback, param, initialNotify);
}
}  // extern "C"
