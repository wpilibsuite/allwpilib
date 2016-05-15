/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Vision/Threshold.h"

Threshold::Threshold(int new_plane1Low, int new_plane1High, int new_plane2Low,
                     int new_plane2High, int new_plane3Low,
                     int new_plane3High) {
  plane1Low = new_plane1Low;
  plane1High = new_plane1High;
  plane2Low = new_plane2Low;
  plane2High = new_plane2High;
  plane3Low = new_plane3Low;
  plane3High = new_plane3High;
}
