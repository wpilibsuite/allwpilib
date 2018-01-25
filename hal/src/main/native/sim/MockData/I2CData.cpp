/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "../PortsInternal.h"
#include "I2CDataInternal.h"
#include "MockData/NotifyCallbackHelpers.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeI2CData() {
  static I2CData sid[2];
  ::hal::SimI2CData = sid;
}
}  // namespace init
}  // namespace hal

I2CData* hal::SimI2CData;

void I2CData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_readCallbacks = nullptr;
}

I2CData::I2CData() {}
I2CData::~I2CData() {}

///////////////////////////////////////////
// Initialize
///////////////////////////////////////////
int32_t I2CData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void I2CData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void I2CData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool I2CData::GetInitialized() { return m_initialized; }

void I2CData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t I2CData::RegisterReadCallback(HAL_BufferCallback callback,
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

void I2CData::CancelReadCallback(int32_t uid) {
  m_readCallbacks = CancelCallback(m_readCallbacks, uid);
}

int32_t I2CData::RegisterWriteCallback(HAL_ConstBufferCallback callback,
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

void I2CData::CancelWriteCallback(int32_t uid) {
  m_writeCallbacks = CancelCallback(m_writeCallbacks, uid);
}

void I2CData::Write(int32_t deviceAddress, const uint8_t* dataToSend,
                    int32_t sendSize) {
  std::lock_guard<wpi::mutex> lock(m_dataMutex);
  InvokeCallback(m_writeCallbacks, "Write", const_cast<uint8_t*>(dataToSend),
                 sendSize);
}
void I2CData::Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) {
  std::lock_guard<wpi::mutex> lock(m_dataMutex);
  InvokeCallback(m_readCallbacks, "Read", buffer, count);
}

extern "C" {
void HALSIM_ResetI2CData(int32_t index) { SimI2CData[index].ResetData(); }

int32_t HALSIM_RegisterI2CInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimI2CData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelI2CInitializedCallback(int32_t index, int32_t uid) {
  SimI2CData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetI2CInitialized(int32_t index) {
  return SimI2CData[index].GetInitialized();
}

void HALSIM_SetI2CInitialized(int32_t index, HAL_Bool initialized) {
  SimI2CData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterI2CReadCallback(int32_t index,
                                       HAL_BufferCallback callback,
                                       void* param) {
  return SimI2CData[index].RegisterReadCallback(callback, param);
}
void HALSIM_CancelI2CReadCallback(int32_t index, int32_t uid) {
  SimI2CData[index].CancelReadCallback(uid);
}

int32_t HALSIM_RegisterI2CWriteCallback(int32_t index,
                                        HAL_ConstBufferCallback callback,
                                        void* param) {
  return SimI2CData[index].RegisterWriteCallback(callback, param);
}
void HALSIM_CancelI2CWriteCallback(int32_t index, int32_t uid) {
  SimI2CData[index].CancelWriteCallback(uid);
}

}  // extern "C"
