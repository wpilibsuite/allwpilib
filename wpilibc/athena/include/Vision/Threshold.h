/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

/**
 * Color threshold values.
 * This object represnts the threshold values for any type of color object
 * that is used in a threshhold operation. It simplifies passing values
 * around in a program for color detection.
 */
class Threshold {
 public:
  int32_t plane1Low;
  int32_t plane1High;
  int32_t plane2Low;
  int32_t plane2High;
  int32_t plane3Low;
  int32_t plane3High;
  Threshold(int32_t plane1Low, int32_t plane1High, int32_t plane2Low,
            int32_t plane2High, int32_t plane3Low, int32_t plane3High);
};
