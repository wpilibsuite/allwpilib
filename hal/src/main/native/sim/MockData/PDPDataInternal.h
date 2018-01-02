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

#include "../PortsInternal.h"
#include "MockData/NotifyListenerVector.h"
#include "MockData/PDPData.h"

namespace hal {
class PDPData {
 public:
  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterTemperatureCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelTemperatureCallback(int32_t uid);
  void InvokeTemperatureCallback(HAL_Value value);
  double GetTemperature();
  void SetTemperature(double temperature);

  int32_t RegisterVoltageCallback(HAL_NotifyCallback callback, void* param,
                                  HAL_Bool initialNotify);
  void CancelVoltageCallback(int32_t uid);
  void InvokeVoltageCallback(HAL_Value value);
  double GetVoltage();
  void SetVoltage(double voltage);

  int32_t RegisterCurrentCallback(int32_t channel, HAL_NotifyCallback callback,
                                  void* param, HAL_Bool initialNotify);
  void CancelCurrentCallback(int32_t channel, int32_t uid);
  void InvokeCurrentCallback(int32_t channel, HAL_Value value);
  double GetCurrent(int32_t channel);
  void SetCurrent(int32_t channel, double current);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::atomic<double> m_temperature{0.0};
  std::shared_ptr<NotifyListenerVector> m_temperatureCallbacks = nullptr;
  std::atomic<double> m_voltage{12.0};
  std::shared_ptr<NotifyListenerVector> m_voltageCallbacks = nullptr;
  std::atomic<double> m_current[kNumPDPChannels];
  std::shared_ptr<NotifyListenerVector> m_currentCallbacks[kNumPDPChannels];
};
extern PDPData* SimPDPData;
}  // namespace hal
