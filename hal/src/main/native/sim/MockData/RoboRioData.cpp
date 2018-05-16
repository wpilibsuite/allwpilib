/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "MockData/NotifyCallbackHelpers.h"
#include "RoboRioDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeRoboRioData() {
  static RoboRioData srrd[1];
  ::hal::SimRoboRioData = srrd;
}
}  // namespace init
}  // namespace hal

RoboRioData* hal::SimRoboRioData;
void RoboRioData::ResetData() {
  m_fPGAButton = false;
  m_fPGAButtonCallbacks = nullptr;
  m_vInVoltage = 0.0;
  m_vInVoltageCallbacks = nullptr;
  m_vInCurrent = 0.0;
  m_vInCurrentCallbacks = nullptr;
  m_userVoltage6V = 6.0;
  m_userVoltage6VCallbacks = nullptr;
  m_userCurrent6V = 0.0;
  m_userCurrent6VCallbacks = nullptr;
  m_userActive6V = false;
  m_userActive6VCallbacks = nullptr;
  m_userVoltage5V = 5.0;
  m_userVoltage5VCallbacks = nullptr;
  m_userCurrent5V = 0.0;
  m_userCurrent5VCallbacks = nullptr;
  m_userActive5V = false;
  m_userActive5VCallbacks = nullptr;
  m_userVoltage3V3 = 3.3;
  m_userVoltage3V3Callbacks = nullptr;
  m_userCurrent3V3 = 0.0;
  m_userCurrent3V3Callbacks = nullptr;
  m_userActive3V3 = false;
  m_userActive3V3Callbacks = nullptr;
  m_userFaults6V = 0;
  m_userFaults6VCallbacks = nullptr;
  m_userFaults5V = 0;
  m_userFaults5VCallbacks = nullptr;
  m_userFaults3V3 = 0;
  m_userFaults3V3Callbacks = nullptr;
}

int32_t RoboRioData::RegisterFPGAButtonCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_fPGAButtonCallbacks = RegisterCallback(
        m_fPGAButtonCallbacks, "FPGAButton", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetFPGAButton());
    callback("FPGAButton", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelFPGAButtonCallback(int32_t uid) {
  m_fPGAButtonCallbacks = CancelCallback(m_fPGAButtonCallbacks, uid);
}

void RoboRioData::InvokeFPGAButtonCallback(HAL_Value value) {
  InvokeCallback(m_fPGAButtonCallbacks, "FPGAButton", &value);
}

HAL_Bool RoboRioData::GetFPGAButton() { return m_fPGAButton; }

void RoboRioData::SetFPGAButton(HAL_Bool fPGAButton) {
  HAL_Bool oldValue = m_fPGAButton.exchange(fPGAButton);
  if (oldValue != fPGAButton) {
    InvokeFPGAButtonCallback(MakeBoolean(fPGAButton));
  }
}

int32_t RoboRioData::RegisterVInVoltageCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_vInVoltageCallbacks = RegisterCallback(
        m_vInVoltageCallbacks, "VInVoltage", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetVInVoltage());
    callback("VInVoltage", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelVInVoltageCallback(int32_t uid) {
  m_vInVoltageCallbacks = CancelCallback(m_vInVoltageCallbacks, uid);
}

void RoboRioData::InvokeVInVoltageCallback(HAL_Value value) {
  InvokeCallback(m_vInVoltageCallbacks, "VInVoltage", &value);
}

double RoboRioData::GetVInVoltage() { return m_vInVoltage; }

void RoboRioData::SetVInVoltage(double vInVoltage) {
  double oldValue = m_vInVoltage.exchange(vInVoltage);
  if (oldValue != vInVoltage) {
    InvokeVInVoltageCallback(MakeDouble(vInVoltage));
  }
}

int32_t RoboRioData::RegisterVInCurrentCallback(HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_vInCurrentCallbacks = RegisterCallback(
        m_vInCurrentCallbacks, "VInCurrent", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetVInCurrent());
    callback("VInCurrent", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelVInCurrentCallback(int32_t uid) {
  m_vInCurrentCallbacks = CancelCallback(m_vInCurrentCallbacks, uid);
}

void RoboRioData::InvokeVInCurrentCallback(HAL_Value value) {
  InvokeCallback(m_vInCurrentCallbacks, "VInCurrent", &value);
}

double RoboRioData::GetVInCurrent() { return m_vInCurrent; }

void RoboRioData::SetVInCurrent(double vInCurrent) {
  double oldValue = m_vInCurrent.exchange(vInCurrent);
  if (oldValue != vInCurrent) {
    InvokeVInCurrentCallback(MakeDouble(vInCurrent));
  }
}

int32_t RoboRioData::RegisterUserVoltage6VCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userVoltage6VCallbacks = RegisterCallback(
        m_userVoltage6VCallbacks, "UserVoltage6V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserVoltage6V());
    callback("UserVoltage6V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserVoltage6VCallback(int32_t uid) {
  m_userVoltage6VCallbacks = CancelCallback(m_userVoltage6VCallbacks, uid);
}

void RoboRioData::InvokeUserVoltage6VCallback(HAL_Value value) {
  InvokeCallback(m_userVoltage6VCallbacks, "UserVoltage6V", &value);
}

double RoboRioData::GetUserVoltage6V() { return m_userVoltage6V; }

void RoboRioData::SetUserVoltage6V(double userVoltage6V) {
  double oldValue = m_userVoltage6V.exchange(userVoltage6V);
  if (oldValue != userVoltage6V) {
    InvokeUserVoltage6VCallback(MakeDouble(userVoltage6V));
  }
}

int32_t RoboRioData::RegisterUserCurrent6VCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userCurrent6VCallbacks = RegisterCallback(
        m_userCurrent6VCallbacks, "UserCurrent6V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserCurrent6V());
    callback("UserCurrent6V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserCurrent6VCallback(int32_t uid) {
  m_userCurrent6VCallbacks = CancelCallback(m_userCurrent6VCallbacks, uid);
}

void RoboRioData::InvokeUserCurrent6VCallback(HAL_Value value) {
  InvokeCallback(m_userCurrent6VCallbacks, "UserCurrent6V", &value);
}

double RoboRioData::GetUserCurrent6V() { return m_userCurrent6V; }

void RoboRioData::SetUserCurrent6V(double userCurrent6V) {
  double oldValue = m_userCurrent6V.exchange(userCurrent6V);
  if (oldValue != userCurrent6V) {
    InvokeUserCurrent6VCallback(MakeDouble(userCurrent6V));
  }
}

int32_t RoboRioData::RegisterUserActive6VCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userActive6VCallbacks = RegisterCallback(
        m_userActive6VCallbacks, "UserActive6V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetUserActive6V());
    callback("UserActive6V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserActive6VCallback(int32_t uid) {
  m_userActive6VCallbacks = CancelCallback(m_userActive6VCallbacks, uid);
}

void RoboRioData::InvokeUserActive6VCallback(HAL_Value value) {
  InvokeCallback(m_userActive6VCallbacks, "UserActive6V", &value);
}

HAL_Bool RoboRioData::GetUserActive6V() { return m_userActive6V; }

void RoboRioData::SetUserActive6V(HAL_Bool userActive6V) {
  HAL_Bool oldValue = m_userActive6V.exchange(userActive6V);
  if (oldValue != userActive6V) {
    InvokeUserActive6VCallback(MakeBoolean(userActive6V));
  }
}

int32_t RoboRioData::RegisterUserVoltage5VCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userVoltage5VCallbacks = RegisterCallback(
        m_userVoltage5VCallbacks, "UserVoltage5V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserVoltage5V());
    callback("UserVoltage5V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserVoltage5VCallback(int32_t uid) {
  m_userVoltage5VCallbacks = CancelCallback(m_userVoltage5VCallbacks, uid);
}

void RoboRioData::InvokeUserVoltage5VCallback(HAL_Value value) {
  InvokeCallback(m_userVoltage5VCallbacks, "UserVoltage5V", &value);
}

double RoboRioData::GetUserVoltage5V() { return m_userVoltage5V; }

void RoboRioData::SetUserVoltage5V(double userVoltage5V) {
  double oldValue = m_userVoltage5V.exchange(userVoltage5V);
  if (oldValue != userVoltage5V) {
    InvokeUserVoltage5VCallback(MakeDouble(userVoltage5V));
  }
}

int32_t RoboRioData::RegisterUserCurrent5VCallback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userCurrent5VCallbacks = RegisterCallback(
        m_userCurrent5VCallbacks, "UserCurrent5V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserCurrent5V());
    callback("UserCurrent5V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserCurrent5VCallback(int32_t uid) {
  m_userCurrent5VCallbacks = CancelCallback(m_userCurrent5VCallbacks, uid);
}

void RoboRioData::InvokeUserCurrent5VCallback(HAL_Value value) {
  InvokeCallback(m_userCurrent5VCallbacks, "UserCurrent5V", &value);
}

double RoboRioData::GetUserCurrent5V() { return m_userCurrent5V; }

void RoboRioData::SetUserCurrent5V(double userCurrent5V) {
  double oldValue = m_userCurrent5V.exchange(userCurrent5V);
  if (oldValue != userCurrent5V) {
    InvokeUserCurrent5VCallback(MakeDouble(userCurrent5V));
  }
}

int32_t RoboRioData::RegisterUserActive5VCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userActive5VCallbacks = RegisterCallback(
        m_userActive5VCallbacks, "UserActive5V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetUserActive5V());
    callback("UserActive5V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserActive5VCallback(int32_t uid) {
  m_userActive5VCallbacks = CancelCallback(m_userActive5VCallbacks, uid);
}

void RoboRioData::InvokeUserActive5VCallback(HAL_Value value) {
  InvokeCallback(m_userActive5VCallbacks, "UserActive5V", &value);
}

HAL_Bool RoboRioData::GetUserActive5V() { return m_userActive5V; }

void RoboRioData::SetUserActive5V(HAL_Bool userActive5V) {
  HAL_Bool oldValue = m_userActive5V.exchange(userActive5V);
  if (oldValue != userActive5V) {
    InvokeUserActive5VCallback(MakeBoolean(userActive5V));
  }
}

int32_t RoboRioData::RegisterUserVoltage3V3Callback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userVoltage3V3Callbacks = RegisterCallback(
        m_userVoltage3V3Callbacks, "UserVoltage3V3", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserVoltage3V3());
    callback("UserVoltage3V3", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserVoltage3V3Callback(int32_t uid) {
  m_userVoltage3V3Callbacks = CancelCallback(m_userVoltage3V3Callbacks, uid);
}

void RoboRioData::InvokeUserVoltage3V3Callback(HAL_Value value) {
  InvokeCallback(m_userVoltage3V3Callbacks, "UserVoltage3V3", &value);
}

double RoboRioData::GetUserVoltage3V3() { return m_userVoltage3V3; }

void RoboRioData::SetUserVoltage3V3(double userVoltage3V3) {
  double oldValue = m_userVoltage3V3.exchange(userVoltage3V3);
  if (oldValue != userVoltage3V3) {
    InvokeUserVoltage3V3Callback(MakeDouble(userVoltage3V3));
  }
}

int32_t RoboRioData::RegisterUserCurrent3V3Callback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userCurrent3V3Callbacks = RegisterCallback(
        m_userCurrent3V3Callbacks, "UserCurrent3V3", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeDouble(GetUserCurrent3V3());
    callback("UserCurrent3V3", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserCurrent3V3Callback(int32_t uid) {
  m_userCurrent3V3Callbacks = CancelCallback(m_userCurrent3V3Callbacks, uid);
}

void RoboRioData::InvokeUserCurrent3V3Callback(HAL_Value value) {
  InvokeCallback(m_userCurrent3V3Callbacks, "UserCurrent3V3", &value);
}

double RoboRioData::GetUserCurrent3V3() { return m_userCurrent3V3; }

void RoboRioData::SetUserCurrent3V3(double userCurrent3V3) {
  double oldValue = m_userCurrent3V3.exchange(userCurrent3V3);
  if (oldValue != userCurrent3V3) {
    InvokeUserCurrent3V3Callback(MakeDouble(userCurrent3V3));
  }
}

int32_t RoboRioData::RegisterUserActive3V3Callback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userActive3V3Callbacks = RegisterCallback(
        m_userActive3V3Callbacks, "UserActive3V3", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeBoolean(GetUserActive3V3());
    callback("UserActive3V3", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserActive3V3Callback(int32_t uid) {
  m_userActive3V3Callbacks = CancelCallback(m_userActive3V3Callbacks, uid);
}

void RoboRioData::InvokeUserActive3V3Callback(HAL_Value value) {
  InvokeCallback(m_userActive3V3Callbacks, "UserActive3V3", &value);
}

HAL_Bool RoboRioData::GetUserActive3V3() { return m_userActive3V3; }

void RoboRioData::SetUserActive3V3(HAL_Bool userActive3V3) {
  HAL_Bool oldValue = m_userActive3V3.exchange(userActive3V3);
  if (oldValue != userActive3V3) {
    InvokeUserActive3V3Callback(MakeBoolean(userActive3V3));
  }
}

int32_t RoboRioData::RegisterUserFaults6VCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userFaults6VCallbacks = RegisterCallback(
        m_userFaults6VCallbacks, "UserFaults6V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetUserFaults6V());
    callback("UserFaults6V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserFaults6VCallback(int32_t uid) {
  m_userFaults6VCallbacks = CancelCallback(m_userFaults6VCallbacks, uid);
}

void RoboRioData::InvokeUserFaults6VCallback(HAL_Value value) {
  InvokeCallback(m_userFaults6VCallbacks, "UserFaults6V", &value);
}

int32_t RoboRioData::GetUserFaults6V() { return m_userFaults6V; }

void RoboRioData::SetUserFaults6V(int32_t userFaults6V) {
  int32_t oldValue = m_userFaults6V.exchange(userFaults6V);
  if (oldValue != userFaults6V) {
    InvokeUserFaults6VCallback(MakeInt(userFaults6V));
  }
}

int32_t RoboRioData::RegisterUserFaults5VCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userFaults5VCallbacks = RegisterCallback(
        m_userFaults5VCallbacks, "UserFaults5V", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetUserFaults5V());
    callback("UserFaults5V", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserFaults5VCallback(int32_t uid) {
  m_userFaults5VCallbacks = CancelCallback(m_userFaults5VCallbacks, uid);
}

void RoboRioData::InvokeUserFaults5VCallback(HAL_Value value) {
  InvokeCallback(m_userFaults5VCallbacks, "UserFaults5V", &value);
}

int32_t RoboRioData::GetUserFaults5V() { return m_userFaults5V; }

void RoboRioData::SetUserFaults5V(int32_t userFaults5V) {
  int32_t oldValue = m_userFaults5V.exchange(userFaults5V);
  if (oldValue != userFaults5V) {
    InvokeUserFaults5VCallback(MakeInt(userFaults5V));
  }
}

int32_t RoboRioData::RegisterUserFaults3V3Callback(HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  // Must return -1 on a null callback for error handling
  if (callback == nullptr) return -1;
  int32_t newUid = 0;
  {
    std::lock_guard<wpi::mutex> lock(m_registerMutex);
    m_userFaults3V3Callbacks = RegisterCallback(
        m_userFaults3V3Callbacks, "UserFaults3V3", callback, param, &newUid);
  }
  if (initialNotify) {
    // We know that the callback is not null because of earlier null check
    HAL_Value value = MakeInt(GetUserFaults3V3());
    callback("UserFaults3V3", param, &value);
  }
  return newUid;
}

void RoboRioData::CancelUserFaults3V3Callback(int32_t uid) {
  m_userFaults3V3Callbacks = CancelCallback(m_userFaults3V3Callbacks, uid);
}

void RoboRioData::InvokeUserFaults3V3Callback(HAL_Value value) {
  InvokeCallback(m_userFaults3V3Callbacks, "UserFaults3V3", &value);
}

int32_t RoboRioData::GetUserFaults3V3() { return m_userFaults3V3; }

void RoboRioData::SetUserFaults3V3(int32_t userFaults3V3) {
  int32_t oldValue = m_userFaults3V3.exchange(userFaults3V3);
  if (oldValue != userFaults3V3) {
    InvokeUserFaults3V3Callback(MakeInt(userFaults3V3));
  }
}

extern "C" {
void HALSIM_ResetRoboRioData(int32_t index) {
  SimRoboRioData[index].ResetData();
}

int32_t HALSIM_RegisterRoboRioFPGAButtonCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterFPGAButtonCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelRoboRioFPGAButtonCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelFPGAButtonCallback(uid);
}

HAL_Bool HALSIM_GetRoboRioFPGAButton(int32_t index) {
  return SimRoboRioData[index].GetFPGAButton();
}

void HALSIM_SetRoboRioFPGAButton(int32_t index, HAL_Bool fPGAButton) {
  SimRoboRioData[index].SetFPGAButton(fPGAButton);
}

int32_t HALSIM_RegisterRoboRioVInVoltageCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterVInVoltageCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelRoboRioVInVoltageCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelVInVoltageCallback(uid);
}

double HALSIM_GetRoboRioVInVoltage(int32_t index) {
  return SimRoboRioData[index].GetVInVoltage();
}

void HALSIM_SetRoboRioVInVoltage(int32_t index, double vInVoltage) {
  SimRoboRioData[index].SetVInVoltage(vInVoltage);
}

int32_t HALSIM_RegisterRoboRioVInCurrentCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterVInCurrentCallback(callback, param,
                                                          initialNotify);
}

void HALSIM_CancelRoboRioVInCurrentCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelVInCurrentCallback(uid);
}

double HALSIM_GetRoboRioVInCurrent(int32_t index) {
  return SimRoboRioData[index].GetVInCurrent();
}

void HALSIM_SetRoboRioVInCurrent(int32_t index, double vInCurrent) {
  SimRoboRioData[index].SetVInCurrent(vInCurrent);
}

int32_t HALSIM_RegisterRoboRioUserVoltage6VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserVoltage6VCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserVoltage6VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserVoltage6VCallback(uid);
}

double HALSIM_GetRoboRioUserVoltage6V(int32_t index) {
  return SimRoboRioData[index].GetUserVoltage6V();
}

void HALSIM_SetRoboRioUserVoltage6V(int32_t index, double userVoltage6V) {
  SimRoboRioData[index].SetUserVoltage6V(userVoltage6V);
}

int32_t HALSIM_RegisterRoboRioUserCurrent6VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserCurrent6VCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserCurrent6VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserCurrent6VCallback(uid);
}

double HALSIM_GetRoboRioUserCurrent6V(int32_t index) {
  return SimRoboRioData[index].GetUserCurrent6V();
}

void HALSIM_SetRoboRioUserCurrent6V(int32_t index, double userCurrent6V) {
  SimRoboRioData[index].SetUserCurrent6V(userCurrent6V);
}

int32_t HALSIM_RegisterRoboRioUserActive6VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserActive6VCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelRoboRioUserActive6VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserActive6VCallback(uid);
}

HAL_Bool HALSIM_GetRoboRioUserActive6V(int32_t index) {
  return SimRoboRioData[index].GetUserActive6V();
}

void HALSIM_SetRoboRioUserActive6V(int32_t index, HAL_Bool userActive6V) {
  SimRoboRioData[index].SetUserActive6V(userActive6V);
}

int32_t HALSIM_RegisterRoboRioUserVoltage5VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserVoltage5VCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserVoltage5VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserVoltage5VCallback(uid);
}

double HALSIM_GetRoboRioUserVoltage5V(int32_t index) {
  return SimRoboRioData[index].GetUserVoltage5V();
}

void HALSIM_SetRoboRioUserVoltage5V(int32_t index, double userVoltage5V) {
  SimRoboRioData[index].SetUserVoltage5V(userVoltage5V);
}

int32_t HALSIM_RegisterRoboRioUserCurrent5VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserCurrent5VCallback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserCurrent5VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserCurrent5VCallback(uid);
}

double HALSIM_GetRoboRioUserCurrent5V(int32_t index) {
  return SimRoboRioData[index].GetUserCurrent5V();
}

void HALSIM_SetRoboRioUserCurrent5V(int32_t index, double userCurrent5V) {
  SimRoboRioData[index].SetUserCurrent5V(userCurrent5V);
}

int32_t HALSIM_RegisterRoboRioUserActive5VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserActive5VCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelRoboRioUserActive5VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserActive5VCallback(uid);
}

HAL_Bool HALSIM_GetRoboRioUserActive5V(int32_t index) {
  return SimRoboRioData[index].GetUserActive5V();
}

void HALSIM_SetRoboRioUserActive5V(int32_t index, HAL_Bool userActive5V) {
  SimRoboRioData[index].SetUserActive5V(userActive5V);
}

int32_t HALSIM_RegisterRoboRioUserVoltage3V3Callback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserVoltage3V3Callback(callback, param,
                                                              initialNotify);
}

void HALSIM_CancelRoboRioUserVoltage3V3Callback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserVoltage3V3Callback(uid);
}

double HALSIM_GetRoboRioUserVoltage3V3(int32_t index) {
  return SimRoboRioData[index].GetUserVoltage3V3();
}

void HALSIM_SetRoboRioUserVoltage3V3(int32_t index, double userVoltage3V3) {
  SimRoboRioData[index].SetUserVoltage3V3(userVoltage3V3);
}

int32_t HALSIM_RegisterRoboRioUserCurrent3V3Callback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserCurrent3V3Callback(callback, param,
                                                              initialNotify);
}

void HALSIM_CancelRoboRioUserCurrent3V3Callback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserCurrent3V3Callback(uid);
}

double HALSIM_GetRoboRioUserCurrent3V3(int32_t index) {
  return SimRoboRioData[index].GetUserCurrent3V3();
}

void HALSIM_SetRoboRioUserCurrent3V3(int32_t index, double userCurrent3V3) {
  SimRoboRioData[index].SetUserCurrent3V3(userCurrent3V3);
}

int32_t HALSIM_RegisterRoboRioUserActive3V3Callback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserActive3V3Callback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserActive3V3Callback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserActive3V3Callback(uid);
}

HAL_Bool HALSIM_GetRoboRioUserActive3V3(int32_t index) {
  return SimRoboRioData[index].GetUserActive3V3();
}

void HALSIM_SetRoboRioUserActive3V3(int32_t index, HAL_Bool userActive3V3) {
  SimRoboRioData[index].SetUserActive3V3(userActive3V3);
}

int32_t HALSIM_RegisterRoboRioUserFaults6VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserFaults6VCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelRoboRioUserFaults6VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserFaults6VCallback(uid);
}

int32_t HALSIM_GetRoboRioUserFaults6V(int32_t index) {
  return SimRoboRioData[index].GetUserFaults6V();
}

void HALSIM_SetRoboRioUserFaults6V(int32_t index, int32_t userFaults6V) {
  SimRoboRioData[index].SetUserFaults6V(userFaults6V);
}

int32_t HALSIM_RegisterRoboRioUserFaults5VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserFaults5VCallback(callback, param,
                                                            initialNotify);
}

void HALSIM_CancelRoboRioUserFaults5VCallback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserFaults5VCallback(uid);
}

int32_t HALSIM_GetRoboRioUserFaults5V(int32_t index) {
  return SimRoboRioData[index].GetUserFaults5V();
}

void HALSIM_SetRoboRioUserFaults5V(int32_t index, int32_t userFaults5V) {
  SimRoboRioData[index].SetUserFaults5V(userFaults5V);
}

int32_t HALSIM_RegisterRoboRioUserFaults3V3Callback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify) {
  return SimRoboRioData[index].RegisterUserFaults3V3Callback(callback, param,
                                                             initialNotify);
}

void HALSIM_CancelRoboRioUserFaults3V3Callback(int32_t index, int32_t uid) {
  SimRoboRioData[index].CancelUserFaults3V3Callback(uid);
}

int32_t HALSIM_GetRoboRioUserFaults3V3(int32_t index) {
  return SimRoboRioData[index].GetUserFaults3V3();
}

void HALSIM_SetRoboRioUserFaults3V3(int32_t index, int32_t userFaults3V3) {
  SimRoboRioData[index].SetUserFaults3V3(userFaults3V3);
}

void HALSIM_RegisterRoboRioAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  SimRoboRioData[index].RegisterFPGAButtonCallback(callback, param,
                                                   initialNotify);
  SimRoboRioData[index].RegisterVInVoltageCallback(callback, param,
                                                   initialNotify);
  SimRoboRioData[index].RegisterVInCurrentCallback(callback, param,
                                                   initialNotify);
  SimRoboRioData[index].RegisterUserVoltage6VCallback(callback, param,
                                                      initialNotify);
  SimRoboRioData[index].RegisterUserCurrent6VCallback(callback, param,
                                                      initialNotify);
  SimRoboRioData[index].RegisterUserActive6VCallback(callback, param,
                                                     initialNotify);
  SimRoboRioData[index].RegisterUserVoltage5VCallback(callback, param,
                                                      initialNotify);
  SimRoboRioData[index].RegisterUserCurrent5VCallback(callback, param,
                                                      initialNotify);
  SimRoboRioData[index].RegisterUserActive5VCallback(callback, param,
                                                     initialNotify);
  SimRoboRioData[index].RegisterUserVoltage3V3Callback(callback, param,
                                                       initialNotify);
  SimRoboRioData[index].RegisterUserCurrent3V3Callback(callback, param,
                                                       initialNotify);
  SimRoboRioData[index].RegisterUserActive3V3Callback(callback, param,
                                                      initialNotify);
  SimRoboRioData[index].RegisterUserFaults6VCallback(callback, param,
                                                     initialNotify);
  SimRoboRioData[index].RegisterUserFaults5VCallback(callback, param,
                                                     initialNotify);
  SimRoboRioData[index].RegisterUserFaults3V3Callback(callback, param,
                                                      initialNotify);
}
}  // extern "C"
