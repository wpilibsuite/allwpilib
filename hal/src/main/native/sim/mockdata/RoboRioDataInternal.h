// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/RoboRioData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class RoboRioData {
  HAL_SIMDATAVALUE_DEFINE_NAME(FPGAButton)
  HAL_SIMDATAVALUE_DEFINE_NAME(VInVoltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(VInCurrent)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserVoltage6V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserCurrent6V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserActive6V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserVoltage5V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserCurrent5V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserActive5V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserVoltage3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserCurrent3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserActive3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserFaults6V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserFaults5V)
  HAL_SIMDATAVALUE_DEFINE_NAME(UserFaults3V3)
  HAL_SIMDATAVALUE_DEFINE_NAME(BrownoutVoltage)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetFPGAButtonName> fpgaButton{false};
  SimDataValue<double, HAL_MakeDouble, GetVInVoltageName> vInVoltage{12.0};
  SimDataValue<double, HAL_MakeDouble, GetVInCurrentName> vInCurrent{0.0};
  SimDataValue<double, HAL_MakeDouble, GetUserVoltage6VName> userVoltage6V{6.0};
  SimDataValue<double, HAL_MakeDouble, GetUserCurrent6VName> userCurrent6V{0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetUserActive6VName> userActive6V{
      true};
  SimDataValue<double, HAL_MakeDouble, GetUserVoltage5VName> userVoltage5V{5.0};
  SimDataValue<double, HAL_MakeDouble, GetUserCurrent5VName> userCurrent5V{0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetUserActive5VName> userActive5V{
      true};
  SimDataValue<double, HAL_MakeDouble, GetUserVoltage3V3Name> userVoltage3V3{
      3.3};
  SimDataValue<double, HAL_MakeDouble, GetUserCurrent3V3Name> userCurrent3V3{
      0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetUserActive3V3Name> userActive3V3{
      true};
  SimDataValue<int32_t, HAL_MakeInt, GetUserFaults6VName> userFaults6V{0};
  SimDataValue<int32_t, HAL_MakeInt, GetUserFaults5VName> userFaults5V{0};
  SimDataValue<int32_t, HAL_MakeInt, GetUserFaults3V3Name> userFaults3V3{0};
  SimDataValue<double, HAL_MakeDouble, GetBrownoutVoltageName> brownoutVoltage{
      6.75};

  virtual void ResetData();
};
extern RoboRioData* SimRoboRioData;
}  // namespace hal
