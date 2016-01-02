/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ImageBase.h"

#include <vector>

class MonoImage : public ImageBase {
 public:
  MonoImage();
  virtual ~MonoImage() = default;

  std::vector<EllipseMatch> *DetectEllipses(
      EllipseDescriptor *ellipseDescriptor, CurveOptions *curveOptions,
      ShapeDetectionOptions *shapeDetectionOptions, ROI *roi);
  std::vector<EllipseMatch> *DetectEllipses(
      EllipseDescriptor *ellipseDescriptor);
};
