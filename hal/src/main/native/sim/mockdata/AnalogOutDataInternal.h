/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/AnalogOutData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class AnalogOutData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)

 public:
  SimDataValue<double, MakeDouble, GetVoltageName> voltage{0.0};
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{0};

  virtual void ResetData();
};
extern AnalogOutData* SimAnalogOutData;
}  // namespace hal
