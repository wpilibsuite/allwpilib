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

#include "MockData/DigitalPWMData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
class DigitalPWMData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterDutyCycleCallback(HAL_NotifyCallback callback, void* param,
                                    HAL_Bool initialNotify);
  void CancelDutyCycleCallback(int32_t uid);
  void InvokeDutyCycleCallback(HAL_Value value);
  double GetDutyCycle();
  void SetDutyCycle(double dutyCycle);

  int32_t RegisterPinCallback(HAL_NotifyCallback callback, void* param,
                              HAL_Bool initialNotify);
  void CancelPinCallback(int32_t uid);
  void InvokePinCallback(HAL_Value value);
  int32_t GetPin();
  void SetPin(int32_t pin);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<double> m_dutyCycle{false};
  std::shared_ptr<NotifyListenerVector> m_dutyCycleCallbacks = nullptr;
  std::atomic<int32_t> m_pin{0};
  std::shared_ptr<NotifyListenerVector> m_pinCallbacks = nullptr;
};
extern DigitalPWMData* SimDigitalPWMData;
}  // namespace hal
