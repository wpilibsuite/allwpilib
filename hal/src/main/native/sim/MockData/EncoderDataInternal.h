/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>
#include <memory>

#include <wpi/mutex.h>

#include "MockData/EncoderData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
class EncoderData {
 public:
  void SetDigitalChannelA(int16_t channel);
  int16_t GetDigitalChannelA();

  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterCountCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelCountCallback(int32_t uid);
  void InvokeCountCallback(HAL_Value value);
  int32_t GetCount();
  void SetCount(int32_t count);

  int32_t RegisterPeriodCallback(HAL_NotifyCallback callback, void* param,
                                 HAL_Bool initialNotify);
  void CancelPeriodCallback(int32_t uid);
  void InvokePeriodCallback(HAL_Value value);
  double GetPeriod();
  void SetPeriod(double period);

  int32_t RegisterResetCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelResetCallback(int32_t uid);
  void InvokeResetCallback(HAL_Value value);
  HAL_Bool GetReset();
  void SetReset(HAL_Bool reset);

  int32_t RegisterMaxPeriodCallback(HAL_NotifyCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelMaxPeriodCallback(int32_t uid);
  void InvokeMaxPeriodCallback(HAL_Value value);
  double GetMaxPeriod();
  void SetMaxPeriod(double maxPeriod);

  int32_t RegisterDirectionCallback(HAL_NotifyCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelDirectionCallback(int32_t uid);
  void InvokeDirectionCallback(HAL_Value value);
  HAL_Bool GetDirection();
  void SetDirection(HAL_Bool direction);

  int32_t RegisterReverseDirectionCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelReverseDirectionCallback(int32_t uid);
  void InvokeReverseDirectionCallback(HAL_Value value);
  HAL_Bool GetReverseDirection();
  void SetReverseDirection(HAL_Bool reverseDirection);

  int32_t RegisterSamplesToAverageCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelSamplesToAverageCallback(int32_t uid);
  void InvokeSamplesToAverageCallback(HAL_Value value);
  int32_t GetSamplesToAverage();
  void SetSamplesToAverage(int32_t samplesToAverage);

  int32_t RegisterDistancePerPulseCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelDistancePerPulseCallback(int32_t uid);
  void InvokeDistancePerPulseCallback(HAL_Value value);
  double GetDistancePerPulse();
  void SetDistancePerPulse(double distancePerPulse);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<int16_t> m_digitalChannelA{0};
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<int32_t> m_count{0};
  std::shared_ptr<NotifyListenerVector> m_countCallbacks = nullptr;
  std::atomic<double> m_period{std::numeric_limits<double>::max()};
  std::shared_ptr<NotifyListenerVector> m_periodCallbacks = nullptr;
  std::atomic<HAL_Bool> m_reset{false};
  std::shared_ptr<NotifyListenerVector> m_resetCallbacks = nullptr;
  std::atomic<double> m_maxPeriod{0};
  std::shared_ptr<NotifyListenerVector> m_maxPeriodCallbacks = nullptr;
  std::atomic<HAL_Bool> m_direction{false};
  std::shared_ptr<NotifyListenerVector> m_directionCallbacks = nullptr;
  std::atomic<HAL_Bool> m_reverseDirection{false};
  std::shared_ptr<NotifyListenerVector> m_reverseDirectionCallbacks = nullptr;
  std::atomic<int32_t> m_samplesToAverage{0};
  std::shared_ptr<NotifyListenerVector> m_samplesToAverageCallbacks = nullptr;
  std::atomic<double> m_distancePerPulse{0};
  std::shared_ptr<NotifyListenerVector> m_distancePerPulseCallbacks = nullptr;
};
extern EncoderData* SimEncoderData;
}  // namespace hal
