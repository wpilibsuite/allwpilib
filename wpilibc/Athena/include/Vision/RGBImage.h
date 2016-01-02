/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ColorImage.h"

/**
 * A color image represented in RGB color space at 3 bytes per pixel.
 */
class RGBImage : public ColorImage {
 public:
  RGBImage();
  RGBImage(const char *fileName);
  virtual ~RGBImage() = default;
};
