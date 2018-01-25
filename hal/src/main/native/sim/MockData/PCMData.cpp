/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "PCMDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePCMData() {
  static PCMData spd[kNumPCMModules];
  ::hal::SimPCMData = spd;
}
}  // namespace init
}  // namespace hal

PCMData* hal::SimPCMData;
void PCMData::ResetData() {
  for (int i = 0; i < kNumSolenoidChannels; i++) {
    m_solenoidInitialized[i] = false;
    m_solenoidInitializedCallbacks[i] = nullptr;
    m_solenoidOutput[i] = false;
    m_solenoidOutputCallbacks[i] = nullptr;
  }
  m_compressorInitialized = false;
  m_compressorInitializedCallbacks = nullptr;
  m_compressorOn = false;
  m_compressorOnCallbacks = nullptr;
  m_closedLoopEnabled = true;
  m_closedLoopEnabledCallbacks = nullptr;
  m_pressureSwitch = false;
  m_pressureSwitchCallbacks = nullptr;
  m_compressorCurrent = 0.0;
  m_compressorCurrentCallbacks = nullptr;
}

int32_t PCMData::RegisterSolenoidInitializedCallback(
    int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_solenoidInitializedCallbacks[channel] =
        RegisterCallback(m_solenoidInitializedCallbacks[channel],
                         "SolenoidInitialized", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetSolenoidInitialized(channel));
    callback("SolenoidInitialized", param, &value);
  }
  return newUid;
}

void PCMData::CancelSolenoidInitializedCallback(int32_t channel, int32_t uid) {
  m_solenoidInitializedCallbacks[channel] =
      CancelCallback(m_solenoidInitializedCallbacks[channel], uid);
}

void PCMData::InvokeSolenoidInitializedCallback(int32_t channel,
                                                HAL_Value value) {
  InvokeCallback(m_solenoidInitializedCallbacks[channel], "SolenoidInitialized",
                 &value);
}

HAL_Bool PCMData::GetSolenoidInitialized(int32_t channel) {
  return m_solenoidInitialized[channel];
}

void PCMData::SetSolenoidInitialized(int32_t channel,
                                     HAL_Bool solenoidInitialized) {
  HAL_Bool oldValue =
      m_solenoidInitialized[channel].exchange(solenoidInitialized);
  if (oldValue != solenoidInitialized) {
    InvokeSolenoidInitializedCallback(channel,
                                      MakeBoolean(solenoidInitialized));
  }
}

int32_t PCMData::RegisterSolenoidOutputCallback(int32_t channel,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_solenoidOutputCallbacks[channel] =
        RegisterCallback(m_solenoidOutputCallbacks[channel], "SolenoidOutput",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetSolenoidOutput(channel));
    callback("SolenoidOutput", param, &value);
  }
  return newUid;
}

void PCMData::CancelSolenoidOutputCallback(int32_t channel, int32_t uid) {
  m_solenoidOutputCallbacks[channel] =
      CancelCallback(m_solenoidOutputCallbacks[channel], uid);
}

void PCMData::InvokeSolenoidOutputCallback(int32_t channel, HAL_Value value) {
  InvokeCallback(m_solenoidOutputCallbacks[channel], "SolenoidOutput", &value);
}

HAL_Bool PCMData::GetSolenoidOutput(int32_t channel) {
  return m_solenoidOutput[channel];
}

void PCMData::SetSolenoidOutput(int32_t channel, HAL_Bool solenoidOutput) {
  HAL_Bool oldValue = m_solenoidOutput[channel].exchange(solenoidOutput);
  if (oldValue != solenoidOutput) {
    InvokeSolenoidOutputCallback(channel, MakeBoolean(solenoidOutput));
  }
}

int32_t PCMData::RegisterCompressorInitializedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_compressorInitializedCallbacks =
        RegisterCallback(m_compressorInitializedCallbacks,
                         "CompressorInitialized", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetCompressorInitialized());
    callback("CompressorInitialized", param, &value);
  }
  return newUid;
}

void PCMData::CancelCompressorInitializedCallback(int32_t uid) {
  m_compressorInitializedCallbacks =
      CancelCallback(m_compressorInitializedCallbacks, uid);
}

void PCMData::InvokeCompressorInitializedCallback(HAL_Value value) {
  InvokeCallback(m_compressorInitializedCallbacks, "CompressorInitialized",
                 &value);
}

HAL_Bool PCMData::GetCompressorInitialized() { return m_compressorInitialized; }

void PCMData::SetCompressorInitialized(HAL_Bool compressorInitialized) {
  HAL_Bool oldValue = m_compressorInitialized.exchange(compressorInitialized);
  if (oldValue != compressorInitialized) {
    InvokeCompressorInitializedCallback(MakeBoolean(compressorInitialized));
  }
}

int32_t PCMData::RegisterCompressorOnCallback(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_compressorOnCallbacks = RegisterCallback(
        m_compressorOnCallbacks, "CompressorOn", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetCompressorOn());
    callback("CompressorOn", param, &value);
  }
  return newUid;
}

void PCMData::CancelCompressorOnCallback(int32_t uid) {
  m_compressorOnCallbacks = CancelCallback(m_compressorOnCallbacks, uid);
}

void PCMData::InvokeCompressorOnCallback(HAL_Value value) {
  InvokeCallback(m_compressorOnCallbacks, "CompressorOn", &value);
}

HAL_Bool PCMData::GetCompressorOn() { return m_compressorOn; }

void PCMData::SetCompressorOn(HAL_Bool compressorOn) {
  HAL_Bool oldValue = m_compressorOn.exchange(compressorOn);
  if (oldValue != compressorOn) {
    InvokeCompressorOnCallback(MakeBoolean(compressorOn));
  }
}

int32_t PCMData::RegisterClosedLoopEnabledCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_closedLoopEnabledCallbacks =
        RegisterCallback(m_closedLoopEnabledCallbacks, "ClosedLoopEnabled",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetClosedLoopEnabled());
    callback("ClosedLoopEnabled", param, &value);
  }
  return newUid;
}

void PCMData::CancelClosedLoopEnabledCallback(int32_t uid) {
  m_closedLoopEnabledCallbacks =
      CancelCallback(m_closedLoopEnabledCallbacks, uid);
}

void PCMData::InvokeClosedLoopEnabledCallback(HAL_Value value) {
  InvokeCallback(m_closedLoopEnabledCallbacks, "ClosedLoopEnabled", &value);
}

HAL_Bool PCMData::GetClosedLoopEnabled() { return m_closedLoopEnabled; }

void PCMData::SetClosedLoopEnabled(HAL_Bool closedLoopEnabled) {
  HAL_Bool oldValue = m_closedLoopEnabled.exchange(closedLoopEnabled);
  if (oldValue != closedLoopEnabled) {
    InvokeClosedLoopEnabledCallback(MakeBoolean(closedLoopEnabled));
  }
}

int32_t PCMData::RegisterPressureSwitchCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_pressureSwitchCallbacks = RegisterCallback(
        m_pressureSwitchCallbacks, "PressureSwitch", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetPressureSwitch());
    callback("PressureSwitch", param, &value);
  }
  return newUid;
}

void PCMData::CancelPressureSwitchCallback(int32_t uid) {
  m_pressureSwitchCallbacks = CancelCallback(m_pressureSwitchCallbacks, uid);
}

void PCMData::InvokePressureSwitchCallback(HAL_Value value) {
  InvokeCallback(m_pressureSwitchCallbacks, "PressureSwitch", &value);
}

HAL_Bool PCMData::GetPressureSwitch() { return m_pressureSwitch; }

void PCMData::SetPressureSwitch(HAL_Bool pressureSwitch) {
  HAL_Bool oldValue = m_pressureSwitch.exchange(pressureSwitch);
  if (oldValue != pressureSwitch) {
    InvokePressureSwitchCallback(MakeBoolean(pressureSwitch));
  }
}

int32_t PCMData::RegisterCompressorCurrentCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_compressorCurrentCallbacks =
        RegisterCallback(m_compressorCurrentCallbacks, "CompressorCurrent",
                         callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetCompressorCurrent());
    callback("CompressorCurrent", param, &value);
  }
  return newUid;
}

void PCMData::CancelCompressorCurrentCallback(int32_t uid) {
  m_compressorCurrentCallbacks =
      CancelCallback(m_compressorCurrentCallbacks, uid);
}

void PCMData::InvokeCompressorCurrentCallback(HAL_Value value) {
  InvokeCallback(m_compressorCurrentCallbacks, "CompressorCurrent", &value);
}

double PCMData::GetCompressorCurrent() { return m_compressorCurrent; }

void PCMData::SetCompressorCurrent(double compressorCurrent) {
  double oldValue = m_compressorCurrent.exchange(compressorCurrent);
  if (oldValue != compressorCurrent) {
    InvokeCompressorCurrentCallback(MakeDouble(compressorCurrent));
  }
}

extern "C" {
void HALSIM_ResetPCMData(int32_t index) { SimPCMData[index].ResetData(); }

int32_t HALSIM_RegisterPCMSolenoidInitializedCallback(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterSolenoidInitializedCallback(
      channel, callback, param, initialNotify);
}

void HALSIM_CancelPCMSolenoidInitializedCallback(int32_t index, int32_t channel,
                                                 int32_t uid) {
  SimPCMData[index].CancelSolenoidInitializedCallback(channel, uid);
}

HAL_Bool HALSIM_GetPCMSolenoidInitialized(int32_t index, int32_t channel) {
  return SimPCMData[index].GetSolenoidInitialized(channel);
}

void HALSIM_SetPCMSolenoidInitialized(int32_t index, int32_t channel,
                                      HAL_Bool solenoidInitialized) {
  SimPCMData[index].SetSolenoidInitialized(channel, solenoidInitialized);
}

int32_t HALSIM_RegisterPCMSolenoidOutputCallback(int32_t index, int32_t channel,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterSolenoidOutputCallback(channel, callback,
                                                          param, initialNotify);
}

void HALSIM_CancelPCMSolenoidOutputCallback(int32_t index, int32_t channel,
                                            int32_t uid) {
  SimPCMData[index].CancelSolenoidOutputCallback(channel, uid);
}

HAL_Bool HALSIM_GetPCMSolenoidOutput(int32_t index, int32_t channel) {
  return SimPCMData[index].GetSolenoidOutput(channel);
}

void HALSIM_SetPCMSolenoidOutput(int32_t index, int32_t channel,
                                 HAL_Bool solenoidOutput) {
  SimPCMData[index].SetSolenoidOutput(channel, solenoidOutput);
}

int32_t HALSIM_RegisterPCMCompressorInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterCompressorInitializedCallback(
      callback, param, initialNotify);
}

void HALSIM_CancelPCMCompressorInitializedCallback(int32_t index, int32_t uid) {
  SimPCMData[index].CancelCompressorInitializedCallback(uid);
}

HAL_Bool HALSIM_GetPCMCompressorInitialized(int32_t index) {
  return SimPCMData[index].GetCompressorInitialized();
}

void HALSIM_SetPCMCompressorInitialized(int32_t index,
                                        HAL_Bool compressorInitialized) {
  SimPCMData[index].SetCompressorInitialized(compressorInitialized);
}

int32_t HALSIM_RegisterPCMCompressorOnCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterCompressorOnCallback(callback, param,
                                                        initialNotify);
}

void HALSIM_CancelPCMCompressorOnCallback(int32_t index, int32_t uid) {
  SimPCMData[index].CancelCompressorOnCallback(uid);
}

HAL_Bool HALSIM_GetPCMCompressorOn(int32_t index) {
  return SimPCMData[index].GetCompressorOn();
}

void HALSIM_SetPCMCompressorOn(int32_t index, HAL_Bool compressorOn) {
  SimPCMData[index].SetCompressorOn(compressorOn);
}

int32_t HALSIM_RegisterPCMClosedLoopEnabledCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterClosedLoopEnabledCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelPCMClosedLoopEnabledCallback(int32_t index, int32_t uid) {
  SimPCMData[index].CancelClosedLoopEnabledCallback(uid);
}

HAL_Bool HALSIM_GetPCMClosedLoopEnabled(int32_t index) {
  return SimPCMData[index].GetClosedLoopEnabled();
}

void HALSIM_SetPCMClosedLoopEnabled(int32_t index, HAL_Bool closedLoopEnabled) {
  SimPCMData[index].SetClosedLoopEnabled(closedLoopEnabled);
}

int32_t HALSIM_RegisterPCMPressureSwitchCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterPressureSwitchCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelPCMPressureSwitchCallback(int32_t index, int32_t uid) {
  SimPCMData[index].CancelPressureSwitchCallback(uid);
}

HAL_Bool HALSIM_GetPCMPressureSwitch(int32_t index) {
  return SimPCMData[index].GetPressureSwitch();
}

void HALSIM_SetPCMPressureSwitch(int32_t index, HAL_Bool pressureSwitch) {
  SimPCMData[index].SetPressureSwitch(pressureSwitch);
}

int32_t HALSIM_RegisterPCMCompressorCurrentCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimPCMData[index].RegisterCompressorCurrentCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelPCMCompressorCurrentCallback(int32_t index, int32_t uid) {
  SimPCMData[index].CancelCompressorCurrentCallback(uid);
}

double HALSIM_GetPCMCompressorCurrent(int32_t index) {
  return SimPCMData[index].GetCompressorCurrent();
}

void HALSIM_SetPCMCompressorCurrent(int32_t index, double compressorCurrent) {
  SimPCMData[index].SetCompressorCurrent(compressorCurrent);
}

void HALSIM_RegisterPCMAllNonSolenoidCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {
  SimPCMData[index].RegisterCompressorInitializedCallback(callback, param,
                                                          initialNotify);
  SimPCMData[index].RegisterCompressorOnCallback(callback, param,
                                                 initialNotify);
  SimPCMData[index].RegisterClosedLoopEnabledCallback(callback, param,
                                                      initialNotify);
  SimPCMData[index].RegisterPressureSwitchCallback(callback, param,
                                                   initialNotify);
  SimPCMData[index].RegisterCompressorCurrentCallback(callback, param,
                                                      initialNotify);
}

void HALSIM_RegisterPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {
  SimPCMData[index].RegisterSolenoidInitializedCallback(channel, callback,
                                                        param, initialNotify);
  SimPCMData[index].RegisterSolenoidOutputCallback(channel, callback, param,
                                                   initialNotify);
}
}  // extern "C"
