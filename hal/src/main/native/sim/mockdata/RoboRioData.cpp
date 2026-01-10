// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include "RoboRioDataInternal.hpp"
#include "wpi/util/string.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeRoboRioData() {
  static RoboRioData srrd;
  ::wpi::hal::SimRoboRioData = &srrd;
}
}  // namespace wpi::hal::init

RoboRioData* wpi::hal::SimRoboRioData;
void RoboRioData::ResetData() {
  vInVoltage.Reset(12.0);
  userVoltage3V3.Reset(3.3);
  userCurrent3V3.Reset(0.0);
  userActive3V3.Reset(true);
  userFaults3V3.Reset(0);
  brownoutVoltage.Reset(6.75);
  cpuTemp.Reset(45.0);
  teamNumber.Reset(0);
  m_serialNumber = "";
  m_comments = "";
}

int32_t RoboRioData::RegisterSerialNumberCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  std::scoped_lock lock(m_serialNumberMutex);
  int32_t uid = m_serialNumberCallbacks.Register(callback, param);
  if (initialNotify) {
    callback(GetSerialNumberName(), param, m_serialNumber.c_str(),
             m_serialNumber.size());
  }
  return uid;
}

void RoboRioData::CancelSerialNumberCallback(int32_t uid) {
  m_serialNumberCallbacks.Cancel(uid);
}

void RoboRioData::GetSerialNumber(struct WPI_String* serialNumber) {
  std::scoped_lock lock(m_serialNumberMutex);
  auto write = WPI_AllocateString(serialNumber, m_serialNumber.size());
  m_serialNumber.copy(write, m_serialNumber.size());
}

void RoboRioData::SetSerialNumber(std::string_view serialNumber) {
  // Limit serial number to 8 characters internally- serialnum environment
  // variable is always 8 characters
  if (serialNumber.size() > 8) {
    serialNumber = serialNumber.substr(0, 8);
  }
  std::scoped_lock lock(m_serialNumberMutex);
  m_serialNumber = std::string(serialNumber);
  m_serialNumberCallbacks(m_serialNumber.c_str(), m_serialNumber.size());
}

int32_t RoboRioData::RegisterCommentsCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  std::scoped_lock lock(m_commentsMutex);
  int32_t uid = m_commentsCallbacks.Register(callback, param);
  if (initialNotify) {
    callback(GetCommentsName(), param, m_comments.c_str(),
             m_serialNumber.size());
  }
  return uid;
}

void RoboRioData::CancelCommentsCallback(int32_t uid) {
  m_commentsCallbacks.Cancel(uid);
}

void RoboRioData::GetComments(struct WPI_String* comments) {
  std::scoped_lock lock(m_commentsMutex);
  auto write = WPI_AllocateString(comments, m_comments.size());
  m_comments.copy(write, m_comments.size());
}

void RoboRioData::SetComments(std::string_view comments) {
  if (comments.size() > 64) {
    comments = comments.substr(0, 64);
  }
  std::scoped_lock lock(m_commentsMutex);
  m_comments = std::string(comments);
  m_commentsCallbacks(m_comments.c_str(), m_comments.size());
}

extern "C" {
void HALSIM_ResetRoboRioData(void) {
  SimRoboRioData->ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI_NOINDEX(TYPE, HALSIM, RoboRio##CAPINAME, \
                                       SimRoboRioData, LOWERNAME)

DEFINE_CAPI(double, VInVoltage, vInVoltage)
DEFINE_CAPI(double, UserVoltage3V3, userVoltage3V3)
DEFINE_CAPI(double, UserCurrent3V3, userCurrent3V3)
DEFINE_CAPI(HAL_Bool, UserActive3V3, userActive3V3)
DEFINE_CAPI(int32_t, UserFaults3V3, userFaults3V3)
DEFINE_CAPI(double, BrownoutVoltage, brownoutVoltage)
DEFINE_CAPI(double, CPUTemp, cpuTemp)
DEFINE_CAPI(int32_t, TeamNumber, teamNumber)

int32_t HALSIM_RegisterRoboRioSerialNumberCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  return SimRoboRioData->RegisterSerialNumberCallback(callback, param,
                                                      initialNotify);
}
void HALSIM_CancelRoboRioSerialNumberCallback(int32_t uid) {
  return SimRoboRioData->CancelSerialNumberCallback(uid);
}
void HALSIM_GetRoboRioSerialNumber(struct WPI_String* serialNumber) {
  SimRoboRioData->GetSerialNumber(serialNumber);
}
void HALSIM_SetRoboRioSerialNumber(const struct WPI_String* serialNumber) {
  SimRoboRioData->SetSerialNumber(wpi::util::to_string_view(serialNumber));
}

int32_t HALSIM_RegisterRoboRioCommentsCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  return SimRoboRioData->RegisterCommentsCallback(callback, param,
                                                  initialNotify);
}
void HALSIM_CancelRoboRioCommentsCallback(int32_t uid) {
  SimRoboRioData->CancelCommentsCallback(uid);
}
void HALSIM_GetRoboRioComments(struct WPI_String* comments) {
  SimRoboRioData->GetComments(comments);
}
void HALSIM_SetRoboRioComments(const struct WPI_String* comments) {
  SimRoboRioData->SetComments(wpi::util::to_string_view(comments));
}

void HALSIM_RegisterRoboRioAllCallbacks(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);

#define REGISTER(NAME) \
  SimRoboRioData->NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterRoboRioAllCallbacks(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  REGISTER(vInVoltage);
  REGISTER(userVoltage3V3);
  REGISTER(userCurrent3V3);
  REGISTER(userActive3V3);
  REGISTER(userFaults3V3);
  REGISTER(brownoutVoltage);
  REGISTER(cpuTemp);
}
}  // extern "C"
