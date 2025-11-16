// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <string>

#include "wpi/hal/simulation/RoboRioData.h"
#include "wpi/hal/simulation/SimDataValue.h"
#include "wpi/util/spinlock.hpp"

namespace wpi::hal {
class RoboRioData {
  HAL_SIMDATAVALUE_DEFINE_NAME(VInVoltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserVoltage3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserCurrent3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserActive3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserFaults3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(BrownoutVoltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(CPUTemp)
  HAL_SIMDATAVALUE_DEFINE_NAME(TeamNumber)

  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(SerialNumber)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Comments);

 public:
  SimDataValue<double, HAL_MakeDouble, GetVInVoltageName> vInVoltage{12.0};
  SimDataValue<double, HAL_MakeDouble, GetUserVoltage3V3Name> userVoltage3V3{
      3.3};
  SimDataValue<double, HAL_MakeDouble, GetUserCurrent3V3Name> userCurrent3V3{
      0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetUserActive3V3Name> userActive3V3{
      true};
  SimDataValue<int32_t, HAL_MakeInt, GetUserFaults3V3Name> userFaults3V3{0};
  SimDataValue<double, HAL_MakeDouble, GetBrownoutVoltageName> brownoutVoltage{
      6.75};
  SimDataValue<double, HAL_MakeDouble, GetCPUTempName> cpuTemp{45.0};
  SimDataValue<int32_t, HAL_MakeInt, GetTeamNumberName> teamNumber{0};

  int32_t RegisterSerialNumberCallback(HAL_RoboRioStringCallback callback,
                                       void* param, HAL_Bool initialNotify);
  void CancelSerialNumberCallback(int32_t uid);
  void GetSerialNumber(struct WPI_String* serialNumber);
  void SetSerialNumber(std::string_view serialNumber);

  int32_t RegisterCommentsCallback(HAL_RoboRioStringCallback callback,
                                   void* param, HAL_Bool initialNotify);
  void CancelCommentsCallback(int32_t uid);
  void GetComments(struct WPI_String* comments);
  void SetComments(std::string_view comments);

  virtual void ResetData();

 private:
  wpi::util::spinlock m_serialNumberMutex;
  std::string m_serialNumber;

  wpi::util::spinlock m_commentsMutex;
  std::string m_comments;

  SimCallbackRegistry<HAL_RoboRioStringCallback, GetSerialNumberName>
      m_serialNumberCallbacks;

  SimCallbackRegistry<HAL_RoboRioStringCallback, GetCommentsName>
      m_commentsCallbacks;
};
extern RoboRioData* SimRoboRioData;
}  // namespace wpi::hal
