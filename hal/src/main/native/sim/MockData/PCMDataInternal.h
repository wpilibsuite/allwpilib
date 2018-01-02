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
#include "MockData/PCMData.h"

namespace hal {
class PCMData {
 public:
  int32_t RegisterSolenoidInitializedCallback(int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
  void CancelSolenoidInitializedCallback(int32_t channel, int32_t uid);
  void InvokeSolenoidInitializedCallback(int32_t channel, HAL_Value value);
  HAL_Bool GetSolenoidInitialized(int32_t channel);
  void SetSolenoidInitialized(int32_t channel, HAL_Bool solenoidInitialized);

  int32_t RegisterSolenoidOutputCallback(int32_t channel,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
  void CancelSolenoidOutputCallback(int32_t channel, int32_t uid);
  void InvokeSolenoidOutputCallback(int32_t channel, HAL_Value value);
  HAL_Bool GetSolenoidOutput(int32_t channel);
  void SetSolenoidOutput(int32_t channel, HAL_Bool solenoidOutput);

  int32_t RegisterCompressorInitializedCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify);
  void CancelCompressorInitializedCallback(int32_t uid);
  void InvokeCompressorInitializedCallback(HAL_Value value);
  HAL_Bool GetCompressorInitialized();
  void SetCompressorInitialized(HAL_Bool compressorInitialized);

  int32_t RegisterCompressorOnCallback(HAL_NotifyCallback callback, void* param,
                                       HAL_Bool initialNotify);
  void CancelCompressorOnCallback(int32_t uid);
  void InvokeCompressorOnCallback(HAL_Value value);
  HAL_Bool GetCompressorOn();
  void SetCompressorOn(HAL_Bool compressorOn);

  int32_t RegisterClosedLoopEnabledCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelClosedLoopEnabledCallback(int32_t uid);
  void InvokeClosedLoopEnabledCallback(HAL_Value value);
  HAL_Bool GetClosedLoopEnabled();
  void SetClosedLoopEnabled(HAL_Bool closedLoopEnabled);

  int32_t RegisterPressureSwitchCallback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);
  void CancelPressureSwitchCallback(int32_t uid);
  void InvokePressureSwitchCallback(HAL_Value value);
  HAL_Bool GetPressureSwitch();
  void SetPressureSwitch(HAL_Bool pressureSwitch);

  int32_t RegisterCompressorCurrentCallback(HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
  void CancelCompressorCurrentCallback(int32_t uid);
  void InvokeCompressorCurrentCallback(HAL_Value value);
  double GetCompressorCurrent();
  void SetCompressorCurrent(double compressorCurrent);

  virtual void ResetData();

 private:
  wpi::mutex m_registerMutex;
  std::atomic<HAL_Bool> m_solenoidInitialized[kNumSolenoidChannels];
  std::shared_ptr<NotifyListenerVector>
      m_solenoidInitializedCallbacks[kNumSolenoidChannels];
  std::atomic<HAL_Bool> m_solenoidOutput[kNumSolenoidChannels];
  std::shared_ptr<NotifyListenerVector>
      m_solenoidOutputCallbacks[kNumSolenoidChannels];
  std::atomic<HAL_Bool> m_compressorInitialized{false};
  std::shared_ptr<NotifyListenerVector> m_compressorInitializedCallbacks =
      nullptr;
  std::atomic<HAL_Bool> m_compressorOn{false};
  std::shared_ptr<NotifyListenerVector> m_compressorOnCallbacks = nullptr;
  std::atomic<HAL_Bool> m_closedLoopEnabled{true};
  std::shared_ptr<NotifyListenerVector> m_closedLoopEnabledCallbacks = nullptr;
  std::atomic<HAL_Bool> m_pressureSwitch{false};
  std::shared_ptr<NotifyListenerVector> m_pressureSwitchCallbacks = nullptr;
  std::atomic<double> m_compressorCurrent{0.0};
  std::shared_ptr<NotifyListenerVector> m_compressorCurrentCallbacks = nullptr;
};
extern PCMData* SimPCMData;
}  // namespace hal
