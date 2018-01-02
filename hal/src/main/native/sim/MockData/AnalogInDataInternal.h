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

#include "MockData/AnalogInData.h"
#include "MockData/NotifyListenerVector.h"

namespace hal {
class AnalogInData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterAverageBitsCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelAverageBitsCallback(int32_t uid);
  void InvokeAverageBitsCallback(HAL_Value value);
  int32_t GetAverageBits();
  void SetAverageBits(int32_t averageBits);

  int32_t RegisterOversampleBitsCallback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
  void CancelOversampleBitsCallback(int32_t uid);
  void InvokeOversampleBitsCallback(HAL_Value value);
  int32_t GetOversampleBits();
  void SetOversampleBits(int32_t oversampleBits);

  int32_t RegisterVoltageCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelVoltageCallback(int32_t uid);
  void InvokeVoltageCallback(HAL_Value value);
  double GetVoltage();
  void SetVoltage(double voltage);

  int32_t RegisterAccumulatorInitializedCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
  void CancelAccumulatorInitializedCallback(int32_t uid);
  void InvokeAccumulatorInitializedCallback(HAL_Value value);
  HAL_Bool GetAccumulatorInitialized();
  void SetAccumulatorInitialized(HAL_Bool accumulatorInitialized);

  int32_t RegisterAccumulatorValueCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelAccumulatorValueCallback(int32_t uid);
  void InvokeAccumulatorValueCallback(HAL_Value value);
  int64_t GetAccumulatorValue();
  void SetAccumulatorValue(int64_t accumulatorValue);

  int32_t RegisterAccumulatorCountCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelAccumulatorCountCallback(int32_t uid);
  void InvokeAccumulatorCountCallback(HAL_Value value);
  int64_t GetAccumulatorCount();
  void SetAccumulatorCount(int64_t accumulatorCount);

  int32_t RegisterAccumulatorCenterCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelAccumulatorCenterCallback(int32_t uid);
  void InvokeAccumulatorCenterCallback(HAL_Value value);
  int32_t GetAccumulatorCenter();
  void SetAccumulatorCenter(int32_t accumulatorCenter);

  int32_t RegisterAccumulatorDeadbandCallback(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
  void CancelAccumulatorDeadbandCallback(int32_t uid);
  void InvokeAccumulatorDeadbandCallback(HAL_Value value);
  int32_t GetAccumulatorDeadband();
  void SetAccumulatorDeadband(int32_t accumulatorDeadband);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<int32_t> m_averageBits{7};
  std::shared_ptr<NotifyListenerVector> m_averageBitsCallbacks = nullptr;
  std::atomic<int32_t> m_oversampleBits{0};
  std::shared_ptr<NotifyListenerVector> m_oversampleBitsCallbacks = nullptr;
  std::atomic<double> m_voltage{0.0};
  std::shared_ptr<NotifyListenerVector> m_voltageCallbacks = nullptr;
  std::atomic<HAL_Bool> m_accumulatorInitialized{false};
  std::shared_ptr<NotifyListenerVector> m_accumulatorInitializedCallbacks =
      nullptr;
  std::atomic<int64_t> m_accumulatorValue{0};
  std::shared_ptr<NotifyListenerVector> m_accumulatorValueCallbacks = nullptr;
  std::atomic<int64_t> m_accumulatorCount{0};
  std::shared_ptr<NotifyListenerVector> m_accumulatorCountCallbacks = nullptr;
  std::atomic<int32_t> m_accumulatorCenter{0};
  std::shared_ptr<NotifyListenerVector> m_accumulatorCenterCallbacks = nullptr;
  std::atomic<int32_t> m_accumulatorDeadband{0};
  std::shared_ptr<NotifyListenerVector> m_accumulatorDeadbandCallbacks =
      nullptr;
};
extern AnalogInData* SimAnalogInData;
}  // namespace hal
