/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

enum AccelerometerRange {
  kRange_2G = 0,
  kRange_4G = 1,
  kRange_8G = 2,
};

extern "C" {
void setAccelerometerActive(bool);
void setAccelerometerRange(AccelerometerRange);
double getAccelerometerX();
double getAccelerometerY();
double getAccelerometerZ();
}
