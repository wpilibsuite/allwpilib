/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once

/**
 * Color threshold values.
 * This object represnts the threshold values for any type of color object
 * that is used in a threshhold operation. It simplifies passing values
 * around in a program for color detection.
 */
class Threshold {
 public:
  int plane1Low;
  int plane1High;
  int plane2Low;
  int plane2High;
  int plane3Low;
  int plane3High;
  Threshold(int plane1Low, int plane1High, int plane2Low, int plane2High,
            int plane3Low, int plane3High);
};
