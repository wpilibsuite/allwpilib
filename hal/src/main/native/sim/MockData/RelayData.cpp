/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "RelayDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeRelayData() {
  static RelayData srd[kNumRelayHeaders];
  ::hal::SimRelayData = srd;
}
}  // namespace init
}  // namespace hal

RelayData* hal::SimRelayData;
void RelayData::ResetData() {
  m_initializedForward = false;
  m_initializedForwardCallbacks = nullptr;
  m_initializedReverse = false;
  m_initializedReverseCallbacks = nullptr;
  m_forward = false;
  m_forwardCallbacks = nullptr;
  m_reverse = false;
  m_reverseCallbacks = nullptr;
}

int32_t RelayData::RegisterInitializedForwardCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_initializedForwardCallbacks =
        RegisterCallback(m_initializedForwardCallbacks, "InitializedForward",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetInitializedForward());
    callback("InitializedForward", param, &value);
  }
  return newUid;
}

void RelayData::CancelInitializedForwardCallback(int32_t uid) {
  m_initializedForwardCallbacks =
      CancelCallback(m_initializedForwardCallbacks, uid);
}

void RelayData::InvokeInitializedForwardCallback(HAL_Value value) {
  InvokeCallback(m_initializedForwardCallbacks, "InitializedForward", &value);
}

HAL_Bool RelayData::GetInitializedForward() { return m_initializedForward; }

void RelayData::SetInitializedForward(HAL_Bool initializedForward) {
  HAL_Bool oldValue = m_initializedForward.exchange(initializedForward);
  if (oldValue != initializedForward) {
    InvokeInitializedForwardCallback(MakeBoolean(initializedForward));
  }
}

int32_t RelayData::RegisterInitializedReverseCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_initializedReverseCallbacks =
        RegisterCallback(m_initializedReverseCallbacks, "InitializedReverse",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetInitializedReverse());
    callback("InitializedReverse", param, &value);
  }
  return newUid;
}

void RelayData::CancelInitializedReverseCallback(int32_t uid) {
  m_initializedReverseCallbacks =
      CancelCallback(m_initializedReverseCallbacks, uid);
}

void RelayData::InvokeInitializedReverseCallback(HAL_Value value) {
  InvokeCallback(m_initializedReverseCallbacks, "InitializedReverse", &value);
}

HAL_Bool RelayData::GetInitializedReverse() { return m_initializedReverse; }

void RelayData::SetInitializedReverse(HAL_Bool initializedReverse) {
  HAL_Bool oldValue = m_initializedReverse.exchange(initializedReverse);
  if (oldValue != initializedReverse) {
    InvokeInitializedReverseCallback(MakeBoolean(initializedReverse));
  }
}

int32_t RelayData::RegisterForwardCallback(HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_forwardCallbacks = RegisterCallback(m_forwardCallbacks, "Forward",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetForward());
    callback("Forward", param, &value);
  }
  return newUid;
}

void RelayData::CancelForwardCallback(int32_t uid) {
  m_forwardCallbacks = CancelCallback(m_forwardCallbacks, uid);
}

void RelayData::InvokeForwardCallback(HAL_Value value) {
  InvokeCallback(m_forwardCallbacks, "Forward", &value);
}

HAL_Bool RelayData::GetForward() { return m_forward; }

void RelayData::SetForward(HAL_Bool forward) {
  HAL_Bool oldValue = m_forward.exchange(forward);
  if (oldValue != forward) {
    InvokeForwardCallback(MakeBoolean(forward));
  }
}

int32_t RelayData::RegisterReverseCallback(HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_reverseCallbacks = RegisterCallback(m_reverseCallbacks, "Reverse",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetReverse());
    callback("Reverse", param, &value);
  }
  return newUid;
}

void RelayData::CancelReverseCallback(int32_t uid) {
  m_reverseCallbacks = CancelCallback(m_reverseCallbacks, uid);
}

void RelayData::InvokeReverseCallback(HAL_Value value) {
  InvokeCallback(m_reverseCallbacks, "Reverse", &value);
}

HAL_Bool RelayData::GetReverse() { return m_reverse; }

void RelayData::SetReverse(HAL_Bool reverse) {
  HAL_Bool oldValue = m_reverse.exchange(reverse);
  if (oldValue != reverse) {
    InvokeReverseCallback(MakeBoolean(reverse));
  }
}

extern "C" {
void HALSIM_ResetRelayData(int32_t index) { SimRelayData[index].ResetData(); }

int32_t HALSIM_RegisterRelayInitializedForwardCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimRelayData[index].RegisterInitializedForwardCallback(callback, param,
                                                                initialNotify);
}

void HALSIM_CancelRelayInitializedForwardCallback(int32_t index, int32_t uid) {
  SimRelayData[index].CancelInitializedForwardCallback(uid);
}

HAL_Bool HALSIM_GetRelayInitializedForward(int32_t index) {
  return SimRelayData[index].GetInitializedForward();
}

void HALSIM_SetRelayInitializedForward(int32_t index,
                                       HAL_Bool initializedForward) {
  SimRelayData[index].SetInitializedForward(initializedForward);
}

int32_t HALSIM_RegisterRelayInitializedReverseCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimRelayData[index].RegisterInitializedReverseCallback(callback, param,
                                                                initialNotify);
}

void HALSIM_CancelRelayInitializedReverseCallback(int32_t index, int32_t uid) {
  SimRelayData[index].CancelInitializedReverseCallback(uid);
}

HAL_Bool HALSIM_GetRelayInitializedReverse(int32_t index) {
  return SimRelayData[index].GetInitializedReverse();
}

void HALSIM_SetRelayInitializedReverse(int32_t index,
                                       HAL_Bool initializedReverse) {
  SimRelayData[index].SetInitializedReverse(initializedReverse);
}

int32_t HALSIM_RegisterRelayForwardCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimRelayData[index].RegisterForwardCallback(callback, param,
                                                     initialNotify);
}

void HALSIM_CancelRelayForwardCallback(int32_t index, int32_t uid) {
  SimRelayData[index].CancelForwardCallback(uid);
}

HAL_Bool HALSIM_GetRelayForward(int32_t index) {
  return SimRelayData[index].GetForward();
}

void HALSIM_SetRelayForward(int32_t index, HAL_Bool forward) {
  SimRelayData[index].SetForward(forward);
}

int32_t HALSIM_RegisterRelayReverseCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimRelayData[index].RegisterReverseCallback(callback, param,
                                                     initialNotify);
}

void HALSIM_CancelRelayReverseCallback(int32_t index, int32_t uid) {
  SimRelayData[index].CancelReverseCallback(uid);
}

HAL_Bool HALSIM_GetRelayReverse(int32_t index) {
  return SimRelayData[index].GetReverse();
}

void HALSIM_SetRelayReverse(int32_t index, HAL_Bool reverse) {
  SimRelayData[index].SetReverse(reverse);
}

void HALSIM_RegisterRelayAllCallbacks(int32_t index,
                                      HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify) {
  SimRelayData[index].RegisterInitializedForwardCallback(callback, param,
                                                         initialNotify);
  SimRelayData[index].RegisterInitializedReverseCallback(callback, param,
                                                         initialNotify);
  SimRelayData[index].RegisterForwardCallback(callback, param, initialNotify);
  SimRelayData[index].RegisterReverseCallback(callback, param, initialNotify);
}
}  // extern "C"
