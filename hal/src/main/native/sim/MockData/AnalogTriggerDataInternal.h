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

#include "MockData/AnalogTriggerData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
class AnalogTriggerData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterTriggerLowerBoundCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelTriggerLowerBoundCallback(int32_t uid);
  void InvokeTriggerLowerBoundCallback(HAL_Value value);
  double GetTriggerLowerBound();
  void SetTriggerLowerBound(double triggerLowerBound);

  int32_t RegisterTriggerUpperBoundCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelTriggerUpperBoundCallback(int32_t uid);
  void InvokeTriggerUpperBoundCallback(HAL_Value value);
  double GetTriggerUpperBound();
  void SetTriggerUpperBound(double triggerUpperBound);

  int32_t RegisterTriggerModeCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelTriggerModeCallback(int32_t uid);
  void InvokeTriggerModeCallback(HAL_Value value);
  HALSIM_AnalogTriggerMode GetTriggerMode();
  void SetTriggerMode(HALSIM_AnalogTriggerMode triggerMode);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{0};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<double> m_triggerLowerBound{0};
  std::shared_ptr<NotifyListenerVector> m_triggerLowerBoundCallbacks = nullptr;
  std::atomic<double> m_triggerUpperBound{0};
  std::shared_ptr<NotifyListenerVector> m_triggerUpperBoundCallbacks = nullptr;
  std::atomic<HALSIM_AnalogTriggerMode> m_triggerMode{
      static_cast<HALSIM_AnalogTriggerMode>(0)};
  std::shared_ptr<NotifyListenerVector> m_triggerModeCallbacks = nullptr;
};
extern AnalogTriggerData* SimAnalogTriggerData;
}  // namespace hal
