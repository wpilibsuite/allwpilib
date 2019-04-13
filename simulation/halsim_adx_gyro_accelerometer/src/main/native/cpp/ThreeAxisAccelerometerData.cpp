/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ThreeAxisAccelerometerData.h"
#pragma warning(disable: 4244 4267 4146)

using namespace hal;

ThreeAxisAccelerometerData::ThreeAxisAccelerometerData() {}

ThreeAxisAccelerometerData::~ThreeAxisAccelerometerData() {}
void ThreeAxisAccelerometerData::ResetData() {
  x.Reset(0.0);
  y.Reset(0.0);
  z.Reset(0.0);
}
