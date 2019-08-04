/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/RoboRioData.h"
#include "mockdata/SimDataValue.h"

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

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetFPGAButtonName> fpgaButton{false};
  SimDataValue<double, MakeDouble, GetVInVoltageName> vInVoltage{0.0};
  SimDataValue<double, MakeDouble, GetVInCurrentName> vInCurrent{0.0};
  SimDataValue<double, MakeDouble, GetUserVoltage6VName> userVoltage6V{6.0};
  SimDataValue<double, MakeDouble, GetUserCurrent6VName> userCurrent6V{0.0};
  SimDataValue<HAL_Bool, MakeBoolean, GetUserActive6VName> userActive6V{false};
  SimDataValue<double, MakeDouble, GetUserVoltage5VName> userVoltage5V{5.0};
  SimDataValue<double, MakeDouble, GetUserCurrent5VName> userCurrent5V{0.0};
  SimDataValue<HAL_Bool, MakeBoolean, GetUserActive5VName> userActive5V{false};
  SimDataValue<double, MakeDouble, GetUserVoltage3V3Name> userVoltage3V3{3.3};
  SimDataValue<double, MakeDouble, GetUserCurrent3V3Name> userCurrent3V3{0.0};
  SimDataValue<HAL_Bool, MakeBoolean, GetUserActive3V3Name> userActive3V3{
      false};
  SimDataValue<int32_t, MakeInt, GetUserFaults6VName> userFaults6V{0};
  SimDataValue<int32_t, MakeInt, GetUserFaults5VName> userFaults5V{0};
  SimDataValue<int32_t, MakeInt, GetUserFaults3V3Name> userFaults3V3{0};

  virtual void ResetData();
};
extern RoboRioData* SimRoboRioData;
}  // namespace hal
