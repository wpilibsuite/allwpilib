/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "EncoderDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeEncoderData() {
  static EncoderData sed[kNumEncoders];
  ::hal::SimEncoderData = sed;
}
}  // namespace init
}  // namespace hal

EncoderData* hal::SimEncoderData;
void EncoderData::ResetData() {
  m_digitalChannelA = 0;
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_count = 0;
  m_countCallbacks = nullptr;
  m_period = std::numeric_limits<double>::max();
  m_periodCallbacks = nullptr;
  m_reset = false;
  m_resetCallbacks = nullptr;
  m_maxPeriod = 0;
  m_maxPeriodCallbacks = nullptr;
  m_direction = false;
  m_directionCallbacks = nullptr;
  m_reverseDirection = false;
  m_reverseDirectionCallbacks = nullptr;
  m_samplesToAverage = 0;
  m_samplesToAverageCallbacks = nullptr;
  m_distancePerPulse = 0;
  m_distancePerPulseCallbacks = nullptr;
}

void EncoderData::SetDigitalChannelA(int16_t channel) {
  m_digitalChannelA = channel;
}

int16_t EncoderData::GetDigitalChannelA() { return m_digitalChannelA; }

int32_t EncoderData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void EncoderData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void EncoderData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool EncoderData::GetInitialized() { return m_initialized; }

void EncoderData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t EncoderData::RegisterCountCallback(HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_countCallbacks =
        RegisterCallback(m_countCallbacks, "Count", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetCount());
    callback("Count", param, &value);
  }
  return newUid;
}

void EncoderData::CancelCountCallback(int32_t uid) {
  m_countCallbacks = CancelCallback(m_countCallbacks, uid);
}

void EncoderData::InvokeCountCallback(HAL_Value value) {
  InvokeCallback(m_countCallbacks, "Count", &value);
}

int32_t EncoderData::GetCount() { return m_count; }

void EncoderData::SetCount(int32_t count) {
  int32_t oldValue = m_count.exchange(count);
  if (oldValue != count) {
    InvokeCountCallback(MakeInt(count));
  }
}

int32_t EncoderData::RegisterPeriodCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_periodCallbacks =
        RegisterCallback(m_periodCallbacks, "Period", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetPeriod());
    callback("Period", param, &value);
  }
  return newUid;
}

void EncoderData::CancelPeriodCallback(int32_t uid) {
  m_periodCallbacks = CancelCallback(m_periodCallbacks, uid);
}

void EncoderData::InvokePeriodCallback(HAL_Value value) {
  InvokeCallback(m_periodCallbacks, "Period", &value);
}

double EncoderData::GetPeriod() { return m_period; }

void EncoderData::SetPeriod(double period) {
  double oldValue = m_period.exchange(period);
  if (oldValue != period) {
    InvokePeriodCallback(MakeDouble(period));
  }
}

int32_t EncoderData::RegisterResetCallback(HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_resetCallbacks =
        RegisterCallback(m_resetCallbacks, "Reset", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetReset());
    callback("Reset", param, &value);
  }
  return newUid;
}

void EncoderData::CancelResetCallback(int32_t uid) {
  m_resetCallbacks = CancelCallback(m_resetCallbacks, uid);
}

void EncoderData::InvokeResetCallback(HAL_Value value) {
  InvokeCallback(m_resetCallbacks, "Reset", &value);
}

HAL_Bool EncoderData::GetReset() { return m_reset; }

void EncoderData::SetReset(HAL_Bool reset) {
  HAL_Bool oldValue = m_reset.exchange(reset);
  if (oldValue != reset) {
    InvokeResetCallback(MakeBoolean(reset));
  }
}

int32_t EncoderData::RegisterMaxPeriodCallback(HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_maxPeriodCallbacks = RegisterCallback(m_maxPeriodCallbacks, "MaxPeriod",
                                            callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetMaxPeriod());
    callback("MaxPeriod", param, &value);
  }
  return newUid;
}

void EncoderData::CancelMaxPeriodCallback(int32_t uid) {
  m_maxPeriodCallbacks = CancelCallback(m_maxPeriodCallbacks, uid);
}

void EncoderData::InvokeMaxPeriodCallback(HAL_Value value) {
  InvokeCallback(m_maxPeriodCallbacks, "MaxPeriod", &value);
}

double EncoderData::GetMaxPeriod() { return m_maxPeriod; }

void EncoderData::SetMaxPeriod(double maxPeriod) {
  double oldValue = m_maxPeriod.exchange(maxPeriod);
  if (oldValue != maxPeriod) {
    InvokeMaxPeriodCallback(MakeDouble(maxPeriod));
  }
}

int32_t EncoderData::RegisterDirectionCallback(HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_directionCallbacks = RegisterCallback(m_directionCallbacks, "Direction",
                                            callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetDirection());
    callback("Direction", param, &value);
  }
  return newUid;
}

void EncoderData::CancelDirectionCallback(int32_t uid) {
  m_directionCallbacks = CancelCallback(m_directionCallbacks, uid);
}

void EncoderData::InvokeDirectionCallback(HAL_Value value) {
  InvokeCallback(m_directionCallbacks, "Direction", &value);
}

HAL_Bool EncoderData::GetDirection() { return m_direction; }

void EncoderData::SetDirection(HAL_Bool direction) {
  HAL_Bool oldValue = m_direction.exchange(direction);
  if (oldValue != direction) {
    InvokeDirectionCallback(MakeBoolean(direction));
  }
}

int32_t EncoderData::RegisterReverseDirectionCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_reverseDirectionCallbacks =
        RegisterCallback(m_reverseDirectionCallbacks, "ReverseDirection",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetReverseDirection());
    callback("ReverseDirection", param, &value);
  }
  return newUid;
}

void EncoderData::CancelReverseDirectionCallback(int32_t uid) {
  m_reverseDirectionCallbacks =
      CancelCallback(m_reverseDirectionCallbacks, uid);
}

void EncoderData::InvokeReverseDirectionCallback(HAL_Value value) {
  InvokeCallback(m_reverseDirectionCallbacks, "ReverseDirection", &value);
}

HAL_Bool EncoderData::GetReverseDirection() { return m_reverseDirection; }

void EncoderData::SetReverseDirection(HAL_Bool reverseDirection) {
  HAL_Bool oldValue = m_reverseDirection.exchange(reverseDirection);
  if (oldValue != reverseDirection) {
    InvokeReverseDirectionCallback(MakeBoolean(reverseDirection));
  }
}

int32_t EncoderData::RegisterSamplesToAverageCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_samplesToAverageCallbacks =
        RegisterCallback(m_samplesToAverageCallbacks, "SamplesToAverage",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetSamplesToAverage());
    callback("SamplesToAverage", param, &value);
  }
  return newUid;
}

void EncoderData::CancelSamplesToAverageCallback(int32_t uid) {
  m_samplesToAverageCallbacks =
      CancelCallback(m_samplesToAverageCallbacks, uid);
}

void EncoderData::InvokeSamplesToAverageCallback(HAL_Value value) {
  InvokeCallback(m_samplesToAverageCallbacks, "SamplesToAverage", &value);
}

int32_t EncoderData::GetSamplesToAverage() { return m_samplesToAverage; }

void EncoderData::SetSamplesToAverage(int32_t samplesToAverage) {
  int32_t oldValue = m_samplesToAverage.exchange(samplesToAverage);
  if (oldValue != samplesToAverage) {
    InvokeSamplesToAverageCallback(MakeInt(samplesToAverage));
  }
}

int32_t EncoderData::RegisterDistancePerPulseCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_distancePerPulseCallbacks =
        RegisterCallback(m_distancePerPulseCallbacks, "DistancePerPulse",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetDistancePerPulse());
    callback("DistancePerPulse", param, &value);
  }
  return newUid;
}
void EncoderData::CancelDistancePerPulseCallback(int32_t uid) {
  m_distancePerPulseCallbacks =
      CancelCallback(m_distancePerPulseCallbacks, uid);
}

void EncoderData::InvokeDistancePerPulseCallback(HAL_Value value) {
  InvokeCallback(m_distancePerPulseCallbacks, "DistancePerPulse", &value);
}

double EncoderData::GetDistancePerPulse() { return m_distancePerPulse; }

void EncoderData::SetDistancePerPulse(double distancePerPulse) {
  double oldValue = m_distancePerPulse.exchange(distancePerPulse);
  if (oldValue != distancePerPulse) {
    InvokeDistancePerPulseCallback(MakeDouble(distancePerPulse));
  }
}

extern "C" {
void HALSIM_ResetEncoderData(int32_t index) {
  SimEncoderData[index].ResetData();
}

int16_t HALSIM_GetDigitalChannelA(int index) {
  return SimEncoderData[index].GetDigitalChannelA();
}

int32_t HALSIM_RegisterEncoderInitializedCallback(int32_t index,
                                                  HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterInitializedCallback(callback, param,
                                                           initialNotify);
}

void HALSIM_CancelEncoderInitializedCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetEncoderInitialized(int32_t index) {
  return SimEncoderData[index].GetInitialized();
}

void HALSIM_SetEncoderInitialized(int32_t index, HAL_Bool initialized) {
  SimEncoderData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterEncoderCountCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterCountCallback(callback, param,
                                                     initialNotify);
}

void HALSIM_CancelEncoderCountCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelCountCallback(uid);
}

int32_t HALSIM_GetEncoderCount(int32_t index) {
  return SimEncoderData[index].GetCount();
}

void HALSIM_SetEncoderCount(int32_t index, int32_t count) {
  SimEncoderData[index].SetCount(count);
}

int32_t HALSIM_RegisterEncoderPeriodCallback(int32_t index,
                                             HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterPeriodCallback(callback, param,
                                                      initialNotify);
}

void HALSIM_CancelEncoderPeriodCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelPeriodCallback(uid);
}

double HALSIM_GetEncoderPeriod(int32_t index) {
  return SimEncoderData[index].GetPeriod();
}

void HALSIM_SetEncoderPeriod(int32_t index, double period) {
  SimEncoderData[index].SetPeriod(period);
}

int32_t HALSIM_RegisterEncoderResetCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterResetCallback(callback, param,
                                                     initialNotify);
}

void HALSIM_CancelEncoderResetCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelResetCallback(uid);
}

HAL_Bool HALSIM_GetEncoderReset(int32_t index) {
  return SimEncoderData[index].GetReset();
}

void HALSIM_SetEncoderReset(int32_t index, HAL_Bool reset) {
  SimEncoderData[index].SetReset(reset);
}

int32_t HALSIM_RegisterEncoderMaxPeriodCallback(int32_t index,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterMaxPeriodCallback(callback, param,
                                                         initialNotify);
}

void HALSIM_CancelEncoderMaxPeriodCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelMaxPeriodCallback(uid);
}

double HALSIM_GetEncoderMaxPeriod(int32_t index) {
  return SimEncoderData[index].GetMaxPeriod();
}

void HALSIM_SetEncoderMaxPeriod(int32_t index, double maxPeriod) {
  SimEncoderData[index].SetMaxPeriod(maxPeriod);
}

int32_t HALSIM_RegisterEncoderDirectionCallback(int32_t index,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterDirectionCallback(callback, param,
                                                         initialNotify);
}

void HALSIM_CancelEncoderDirectionCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelDirectionCallback(uid);
}

HAL_Bool HALSIM_GetEncoderDirection(int32_t index) {
  return SimEncoderData[index].GetDirection();
}

void HALSIM_SetEncoderDirection(int32_t index, HAL_Bool direction) {
  SimEncoderData[index].SetDirection(direction);
}

int32_t HALSIM_RegisterEncoderReverseDirectionCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterReverseDirectionCallback(callback, param,
                                                                initialNotify);
}

void HALSIM_CancelEncoderReverseDirectionCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelReverseDirectionCallback(uid);
}

HAL_Bool HALSIM_GetEncoderReverseDirection(int32_t index) {
  return SimEncoderData[index].GetReverseDirection();
}

void HALSIM_SetEncoderReverseDirection(int32_t index,
                                       HAL_Bool reverseDirection) {
  SimEncoderData[index].SetReverseDirection(reverseDirection);
}

int32_t HALSIM_RegisterEncoderSamplesToAverageCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimEncoderData[index].RegisterSamplesToAverageCallback(callback, param,
                                                                initialNotify);
}

void HALSIM_CancelEncoderSamplesToAverageCallback(int32_t index, int32_t uid) {
  SimEncoderData[index].CancelSamplesToAverageCallback(uid);
}

int32_t HALSIM_GetEncoderSamplesToAverage(int32_t index) {
  return SimEncoderData[index].GetSamplesToAverage();
}

void HALSIM_SetEncoderSamplesToAverage(int32_t index,
                                       int32_t samplesToAverage) {
  SimEncoderData[index].SetSamplesToAverage(samplesToAverage);
}

void HALSIM_RegisterEncoderAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  SimEncoderData[index].RegisterInitializedCallback(callback, param,
                                                    initialNotify);
  SimEncoderData[index].RegisterCountCallback(callback, param, initialNotify);
  SimEncoderData[index].RegisterPeriodCallback(callback, param, initialNotify);
  SimEncoderData[index].RegisterResetCallback(callback, param, initialNotify);
  SimEncoderData[index].RegisterMaxPeriodCallback(callback, param,
                                                  initialNotify);
  SimEncoderData[index].RegisterDirectionCallback(callback, param,
                                                  initialNotify);
  SimEncoderData[index].RegisterReverseDirectionCallback(callback, param,
                                                         initialNotify);
  SimEncoderData[index].RegisterSamplesToAverageCallback(callback, param,
                                                         initialNotify);
  SimEncoderData[index].RegisterDistancePerPulseCallback(callback, param,
                                                         initialNotify);
}
}  // extern "C"
