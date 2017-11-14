/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "../PortsInternal.h"
#include "NotifyCallbackHelpers.h"
#include "SPIDataInternal.h"

using namespace hal;

SPIData hal::SimSPIData[5];

void SPIData::ResetData() {
  m_initialized = false;
  m_accumulatorValue = 0;
  m_initializedCallbacks = nullptr;
  m_readCallbacks = nullptr;
  m_writeCallbacks = nullptr;
  m_resetAccumulatorCallback = nullptr;
  m_setAccumulatorCallback = nullptr;
}

SPIData::SPIData() {}
SPIData::~SPIData() {}

int32_t SPIData::RegisterInitializedCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_initializedCallbacks = RegisterCallback(
        m_initializedCallbacks, "Initialized", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetInitialized());
    callback("Initialized", param, &value);
  }
  return newUid;
}

void SPIData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void SPIData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool SPIData::GetInitialized() { return m_initialized; }

void SPIData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t SPIData::RegisterReadCallback(HAL_BufferCallback callback,
                                      void* param) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_readCallbacks =
        RegisterCallback(m_readCallbacks, "Read", callback, param, &newUid);
  }

  return newUid;
}

void SPIData::CancelReadCallback(int32_t uid) {
  m_readCallbacks = CancelCallback(m_readCallbacks, uid);
}

int32_t SPIData::RegisterWriteCallback(HAL_BufferCallback callback,
                                       void* param) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_writeCallbacks =
        RegisterCallback(m_writeCallbacks, "Write", callback, param, &newUid);
  }

  return newUid;
}

void SPIData::CancelWriteCallback(int32_t uid) {
  m_writeCallbacks = CancelCallback(m_writeCallbacks, uid);
}

int32_t SPIData::RegisterResetAccumulatorCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_resetAccumulatorCallback =
        RegisterCallback(m_resetAccumulatorCallback, "ResetAccumulator",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetInitialized());
    callback("ResetAccumulator", param, &value);
  }
  return newUid;
}

void SPIData::CancelResetAccumulatorCallback(int32_t uid) {
  m_resetAccumulatorCallback = CancelCallback(m_resetAccumulatorCallback, uid);
}

int32_t SPIData::RegisterAccumulatorCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_setAccumulatorCallback = RegisterCallback(
        m_setAccumulatorCallback, "SetAccumulator", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetAccumulatorValue());
    callback("SetAccumulator", param, &value);
  }
  return newUid;
}

void SPIData::CancelAccumulatorCallback(int32_t uid) {
  m_setAccumulatorCallback = CancelCallback(m_setAccumulatorCallback, uid);
}

void SPIData::InvokeSetAccumulatorCallback(HAL_Value value) {
  InvokeCallback(m_setAccumulatorCallback, "SetAccumulator", &value);
}

void SPIData::SetAccumulatorValue(int64_t value) {
  int64_t oldValue = m_accumulatorValue.exchange(value);
  if (oldValue != value) {
    InvokeSetAccumulatorCallback(MakeLong(value));
  }
}

int64_t SPIData::GetAccumulatorValue() { return m_accumulatorValue; }

int32_t SPIData::Read(uint8_t* buffer, int32_t count) {
  std::lock_guard<wpi::mutex> lock(m_dataMutex);
  InvokeCallback(m_readCallbacks, "Read", buffer, count);

  return count;
}

int32_t SPIData::Write(const uint8_t* dataToSend, int32_t sendSize) {
  std::lock_guard<wpi::mutex> lock(m_dataMutex);
  InvokeCallback(m_writeCallbacks, "Write", const_cast<uint8_t*>(dataToSend),
                 sendSize);

  return sendSize;
}

int32_t SPIData::Transaction(const uint8_t* dataToSend, uint8_t* dataReceived,
                             int32_t size) {
  std::lock_guard<wpi::mutex> lock(m_dataMutex);
  return size;
}

void SPIData::ResetAccumulator() {
  HAL_Value value = MakeInt(0);
  InvokeCallback(m_resetAccumulatorCallback, "ResetAccumulator", &value);
}

extern "C" {
void HALSIM_ResetSPIData(int32_t index) { SimSPIData[index].ResetData(); }

int32_t HALSIM_RegisterSPIInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimSPIData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelSPIInitializedCallback(int32_t index, int32_t uid) {
  SimSPIData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetSPIInitialized(int32_t index) {
  return SimSPIData[index].GetInitialized();
}

void HALSIM_SetSPIInitialized(int32_t index, HAL_Bool initialized) {
  SimSPIData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterSPIReadCallback(int32_t index,
                                       HAL_BufferCallback callback,
                                       void* param) {
  return SimSPIData[index].RegisterReadCallback(callback, param);
}
void HALSIM_CancelSPIReadCallback(int32_t index, int32_t uid) {
  SimSPIData[index].CancelReadCallback(uid);
}

int32_t HALSIM_RegisterSPIWriteCallback(int32_t index,
                                        HAL_BufferCallback callback,
                                        void* param) {
  return SimSPIData[index].RegisterWriteCallback(callback, param);
}
void HALSIM_CancelSPIWriteCallback(int32_t index, int32_t uid) {
  SimSPIData[index].CancelWriteCallback(uid);
}

int32_t HALSIM_RegisterSPIResetAccumulatorCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimSPIData[index].RegisterResetAccumulatorCallback(callback, param,
                                                            initialNotify);
}
void HALSIM_CancelSPIResetAccumulatorCallback(int32_t index, int32_t uid) {
  SimSPIData[index].CancelResetAccumulatorCallback(uid);
}

void HALSIM_SetSPISetAccumulatorValue(int32_t index, int64_t value) {
  SimSPIData[index].SetAccumulatorValue(value);
}
int64_t HALSIM_GetSPIGetAccumulatorValue(int32_t index) {
  return SimSPIData[index].GetAccumulatorValue();
}

}  // extern "C"
