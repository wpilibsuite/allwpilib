/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>

#include <support/mutex.h>

#include "MockData/NotifyListenerVector.h"
#include "MockData/PWMData.h"

namespace hal {
class PWMData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterRawValueCallback(HAL_NotifyCallback callback, void* param,
                                   HAL_Bool initialNotify);
  void CancelRawValueCallback(int32_t uid);
  void InvokeRawValueCallback(HAL_Value value);
  int32_t GetRawValue();
  void SetRawValue(int32_t rawValue);

  int32_t RegisterSpeedCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelSpeedCallback(int32_t uid);
  void InvokeSpeedCallback(HAL_Value value);
  double GetSpeed();
  void SetSpeed(double speed);

  int32_t RegisterPositionCallback(HAL_NotifyCallback callback, void* param,
                                   HAL_Bool initialNotify);
  void CancelPositionCallback(int32_t uid);
  void InvokePositionCallback(HAL_Value value);
  double GetPosition();
  void SetPosition(double position);

  int32_t RegisterPeriodScaleCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelPeriodScaleCallback(int32_t uid);
  void InvokePeriodScaleCallback(HAL_Value value);
  int32_t GetPeriodScale();
  void SetPeriodScale(int32_t periodScale);

  int32_t RegisterZeroLatchCallback(HAL_NotifyCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelZeroLatchCallback(int32_t uid);
  void InvokeZeroLatchCallback(HAL_Value value);
  HAL_Bool GetZeroLatch();
  void SetZeroLatch(HAL_Bool zeroLatch);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<int32_t> m_rawValue{0};
  std::shared_ptr<NotifyListenerVector> m_rawValueCallbacks = nullptr;
  std::atomic<double> m_speed{0};
  std::shared_ptr<NotifyListenerVector> m_speedCallbacks = nullptr;
  std::atomic<double> m_position{0};
  std::shared_ptr<NotifyListenerVector> m_positionCallbacks = nullptr;
  std::atomic<int32_t> m_periodScale{0};
  std::shared_ptr<NotifyListenerVector> m_periodScaleCallbacks = nullptr;
  std::atomic<HAL_Bool> m_zeroLatch{false};
  std::shared_ptr<NotifyListenerVector> m_zeroLatchCallbacks = nullptr;
};
extern PWMData* SimPWMData;
}  // namespace hal
