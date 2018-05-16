/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "PDPDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePDPData() {
  static PDPData spd[kNumPDPModules];
  ::hal::SimPDPData = spd;
}
}  // namespace init
}  // namespace hal

PDPData* hal::SimPDPData;
void PDPData::ResetData() {
  m_initialized = false;
  m_initializedCallbacks = nullptr;
  m_temperature = 0.0;
  m_temperatureCallbacks = nullptr;
  m_voltage = 12.0;
  m_voltageCallbacks = nullptr;
  for (int i = 0; i < kNumPDPChannels; i++) {
    m_current[i] = 0;
    m_currentCallbacks[i] = nullptr;
  }
}

int32_t PDPData::RegisterInitializedCallback(HAL_NotifyCallback callback,
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

void PDPData::CancelInitializedCallback(int32_t uid) {
  m_initializedCallbacks = CancelCallback(m_initializedCallbacks, uid);
}

void PDPData::InvokeInitializedCallback(HAL_Value value) {
  InvokeCallback(m_initializedCallbacks, "Initialized", &value);
}

HAL_Bool PDPData::GetInitialized() { return m_initialized; }

void PDPData::SetInitialized(HAL_Bool initialized) {
  HAL_Bool oldValue = m_initialized.exchange(initialized);
  if (oldValue != initialized) {
    InvokeInitializedCallback(MakeBoolean(initialized));
  }
}

int32_t PDPData::RegisterTemperatureCallback(HAL_NotifyCallback callback,
                                             void* param,
                                             HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_temperatureCallbacks = RegisterCallback(
        m_temperatureCallbacks, "Temperature", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetTemperature());
    callback("Temperature", param, &value);
  }
  return newUid;
}

void PDPData::CancelTemperatureCallback(int32_t uid) {
  m_temperatureCallbacks = CancelCallback(m_temperatureCallbacks, uid);
}

void PDPData::InvokeTemperatureCallback(HAL_Value value) {
  InvokeCallback(m_temperatureCallbacks, "Temperature", &value);
}

double PDPData::GetTemperature() { return m_temperature; }

void PDPData::SetTemperature(double temperature) {
  double oldValue = m_temperature.exchange(temperature);
  if (oldValue != temperature) {
    InvokeTemperatureCallback(MakeDouble(temperature));
  }
}

int32_t PDPData::RegisterVoltageCallback(HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_voltageCallbacks = RegisterCallback(m_voltageCallbacks, "Voltage",
                                          callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetVoltage());
    callback("Voltage", param, &value);
  }
  return newUid;
}

void PDPData::CancelVoltageCallback(int32_t uid) {
  m_voltageCallbacks = CancelCallback(m_voltageCallbacks, uid);
}

void PDPData::InvokeVoltageCallback(HAL_Value value) {
  InvokeCallback(m_voltageCallbacks, "Voltage", &value);
}

double PDPData::GetVoltage() { return m_voltage; }

void PDPData::SetVoltage(double voltage) {
  double oldValue = m_voltage.exchange(voltage);
  if (oldValue != voltage) {
    InvokeVoltageCallback(MakeDouble(voltage));
  }
}

int32_t PDPData::RegisterCurrentCallback(int32_t channel,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_currentCallbacks[channel] = RegisterCallback(
        m_currentCallbacks[channel], "Current", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetCurrent(channel));
    callback("Current", param, &value);
  }
  return newUid;
}

void PDPData::CancelCurrentCallback(int32_t channel, int32_t uid) {
  m_currentCallbacks[channel] =
      CancelCallback(m_currentCallbacks[channel], uid);
}

void PDPData::InvokeCurrentCallback(int32_t channel, HAL_Value value) {
  InvokeCallback(m_currentCallbacks[channel], "Current", &value);
}

double PDPData::GetCurrent(int32_t channel) { return m_current[channel]; }

void PDPData::SetCurrent(int32_t channel, double current) {
  double oldValue = m_current[channel].exchange(current);
  if (oldValue != current) {
    InvokeCurrentCallback(channel, MakeDouble(current));
  }
}

extern "C" {
void HALSIM_ResetPDPData(int32_t index) { SimPDPData[index].ResetData(); }

int32_t HALSIM_RegisterPDPInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimPDPData[index].RegisterInitializedCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelPDPInitializedCallback(int32_t index, int32_t uid) {
  SimPDPData[index].CancelInitializedCallback(uid);
}

HAL_Bool HALSIM_GetPDPInitialized(int32_t index) {
  return SimPDPData[index].GetInitialized();
}

void HALSIM_SetPDPInitialized(int32_t index, HAL_Bool initialized) {
  SimPDPData[index].SetInitialized(initialized);
}

int32_t HALSIM_RegisterPDPTemperatureCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  return SimPDPData[index].RegisterTemperatureCallback(callback, param,
                                                       initialNotify);
}

void HALSIM_CancelPDPTemperatureCallback(int32_t index, int32_t uid) {
  SimPDPData[index].CancelTemperatureCallback(uid);
}

double HALSIM_GetPDPTemperature(int32_t index) {
  return SimPDPData[index].GetTemperature();
}

void HALSIM_SetPDPTemperature(int32_t index, double temperature) {
  SimPDPData[index].SetTemperature(temperature);
}

int32_t HALSIM_RegisterPDPVoltageCallback(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  return SimPDPData[index].RegisterVoltageCallback(callback, param,
                                                   initialNotify);
}

void HALSIM_CancelPDPVoltageCallback(int32_t index, int32_t uid) {
  SimPDPData[index].CancelVoltageCallback(uid);
}

double HALSIM_GetPDPVoltage(int32_t index) {
  return SimPDPData[index].GetVoltage();
}

void HALSIM_SetPDPVoltage(int32_t index, double voltage) {
  SimPDPData[index].SetVoltage(voltage);
}

int32_t HALSIM_RegisterPDPCurrentCallback(int32_t index, int32_t channel,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  return SimPDPData[index].RegisterCurrentCallback(channel, callback, param,
                                                   initialNotify);
}

void HALSIM_CancelPDPCurrentCallback(int32_t index, int32_t channel,
                                     int32_t uid) {
  SimPDPData[index].CancelCurrentCallback(channel, uid);
}

double HALSIM_GetPDPCurrent(int32_t index, int32_t channel) {
  return SimPDPData[index].GetCurrent(channel);
}

void HALSIM_SetPDPCurrent(int32_t index, int32_t channel, double current) {
  SimPDPData[index].SetCurrent(channel, current);
}

void HALSIM_RegisterPDPAllNonCurrentCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  SimPDPData[index].RegisterInitializedCallback(callback, param, initialNotify);
  SimPDPData[index].RegisterTemperatureCallback(callback, param, initialNotify);
  SimPDPData[index].RegisterVoltageCallback(callback, param, initialNotify);
}
}  // extern "C"
