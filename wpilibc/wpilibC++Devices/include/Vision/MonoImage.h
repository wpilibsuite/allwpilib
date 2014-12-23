/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ImageBase.h"

#include <vector>

class MonoImage : public ImageBase
{
public:
	MonoImage();
	virtual ~MonoImage();

	std::vector<EllipseMatch> *DetectEllipses(EllipseDescriptor *ellipseDescriptor,
	                                          CurveOptions *curveOptions,
	                                          ShapeDetectionOptions *shapeDetectionOptions,
	                                          ROI *roi);
	std::vector<EllipseMatch> * DetectEllipses(EllipseDescriptor *ellipseDescriptor);
};

