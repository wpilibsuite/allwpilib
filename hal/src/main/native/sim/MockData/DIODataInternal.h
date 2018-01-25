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

#include "MockData/DIOData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
class DIOData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterValueCallback(HAL_NotifyCallback callback, void* param,
                                HAL_Bool initialNotify);
  void CancelValueCallback(int32_t uid);
  void InvokeValueCallback(HAL_Value value);
  HAL_Bool GetValue();
  void SetValue(HAL_Bool value);

  int32_t RegisterPulseLengthCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelPulseLengthCallback(int32_t uid);
  void InvokePulseLengthCallback(HAL_Value value);
  double GetPulseLength();
  void SetPulseLength(double pulseLength);

  int32_t RegisterIsInputCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelIsInputCallback(int32_t uid);
  void InvokeIsInputCallback(HAL_Value value);
  HAL_Bool GetIsInput();
  void SetIsInput(HAL_Bool isInput);

  int32_t RegisterFilterIndexCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelFilterIndexCallback(int32_t uid);
  void InvokeFilterIndexCallback(HAL_Value value);
  int32_t GetFilterIndex();
  void SetFilterIndex(int32_t filterIndex);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<HAL_Bool> m_value{true};
  std::shared_ptr<NotifyListenerVector> m_valueCallbacks = nullptr;
  std::atomic<double> m_pulseLength{0.0};
  std::shared_ptr<NotifyListenerVector> m_pulseLengthCallbacks = nullptr;
  std::atomic<HAL_Bool> m_isInput{true};
  std::shared_ptr<NotifyListenerVector> m_isInputCallbacks = nullptr;
  std::atomic<int32_t> m_filterIndex{-1};
  std::shared_ptr<NotifyListenerVector> m_filterIndexCallbacks = nullptr;
};
extern DIOData* SimDIOData;
}  // namespace hal
