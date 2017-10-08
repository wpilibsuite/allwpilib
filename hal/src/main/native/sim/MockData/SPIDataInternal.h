/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>
#include <memory>

#include "MockData/NotifyListenerVector.h"
#include "MockData/SPIData.h"

namespace hal {
class SPIData {
 public:
  SPIData();
  ~SPIData();

  int32_t RegisterInitializedCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelInitializedCallback(int32_t uid);
  void InvokeInitializedCallback(HAL_Value value);
  HAL_Bool GetInitialized();
  void SetInitialized(HAL_Bool initialized);

  int32_t RegisterReadCallback(HAL_BufferCallback callback, void* param);
  void CancelReadCallback(int32_t uid);

  int32_t RegisterWriteCallback(HAL_BufferCallback callback, void* param);
  void CancelWriteCallback(int32_t uid);

  int32_t RegisterResetAccumulatorCallback(HAL_NotifyCallback callback,
                                           void* param, HAL_Bool initialNotify);
  void CancelResetAccumulatorCallback(int32_t uid);

  int32_t RegisterAccumulatorCallback(HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);
  void CancelAccumulatorCallback(int32_t uid);
  void InvokeSetAccumulatorCallback(HAL_Value value);
  void SetAccumulatorValue(int64_t value);
  int64_t GetAccumulatorValue();

  int32_t Read(uint8_t* buffer, int32_t count);
  int32_t Write(uint8_t* dataToSend, int32_t sendSize);
  int32_t Transaction(uint8_t* dataToSend, uint8_t* dataReceived, int32_t size);
  void ResetAccumulator();

  void ResetData();

 private:
  std::mutex m_registerMutex;
  std::mutex m_dataMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::atomic<int64_t> m_accumulatorValue{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::shared_ptr<BufferListenerVector> m_readCallbacks = nullptr;
  std::shared_ptr<BufferListenerVector> m_writeCallbacks = nullptr;
  std::shared_ptr<NotifyListenerVector> m_resetAccumulatorCallback = nullptr;
  std::shared_ptr<NotifyListenerVector> m_setAccumulatorCallback = nullptr;
};
extern SPIData SimSPIData[];
}  // namespace hal
