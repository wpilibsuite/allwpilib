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

#include <support/mutex.h>

#include "MockData/NotifyListenerVector.h"
#include "MockData/SPIData.h"

namespace hal {

typedef HalCallbackListenerVectorImpl<HAL_SpiReadAutoReceiveBufferCallback>
    SpiAutoReceiveDataListenerVector;

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

  int32_t RegisterWriteCallback(HAL_ConstBufferCallback callback, void* param);
  void CancelWriteCallback(int32_t uid);

  int32_t RegisterReadAutoReceivedDataCallback(
      HAL_SpiReadAutoReceiveBufferCallback callback, void* param);
  void CancelReadAutoReceivedDataCallback(int32_t uid);

  int32_t Read(uint8_t* buffer, int32_t count);
  int32_t Write(const uint8_t* dataToSend, int32_t sendSize);
  int32_t Transaction(const uint8_t* dataToSend, uint8_t* dataReceived,
                      int32_t size);

  int32_t ReadAutoReceivedData(uint8_t* buffer, int32_t numToRead,
                               double timeout, int32_t* status);

  void ResetData();

 private:
  wpi::mutex m_registerMutex;
  wpi::mutex m_dataMutex;
  std::atomic<HAL_Bool> m_initialized{false};
  std::shared_ptr<NotifyListenerVector> m_initializedCallbacks = nullptr;
  std::shared_ptr<BufferListenerVector> m_readCallbacks = nullptr;
  std::shared_ptr<ConstBufferListenerVector> m_writeCallbacks = nullptr;
  std::shared_ptr<SpiAutoReceiveDataListenerVector> m_autoReceiveDataCallbacks =
      nullptr;
};
extern SPIData* SimSPIData;
}  // namespace hal
