/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/DigitalPWMData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class DigitalPWMData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(DutyCycle)
  HAL_SIMDATAVALUE_DEFINE_NAME(Pin)

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimDataValue<double, MakeDouble, GetDutyCycleName> dutyCycle{0.0};
  SimDataValue<int32_t, MakeInt, GetPinName> pin{0};

  virtual void ResetData();
};
extern DigitalPWMData* SimDigitalPWMData;
}  // namespace hal
