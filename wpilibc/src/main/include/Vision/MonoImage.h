/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __MONO_IMAGE_H__
#define __MONO_IMAGE_H__

#include "ImageBase.h"

#include <vector>

using namespace std;

class MonoImage : public ImageBase
{
public:
	MonoImage();
	virtual ~MonoImage();

	vector<EllipseMatch> * DetectEllipses(EllipseDescriptor *ellipseDescriptor,
					CurveOptions *curveOptions,
					ShapeDetectionOptions *shapeDetectionOptions,
					ROI *roi);
	vector<EllipseMatch> * DetectEllipses(EllipseDescriptor *ellipseDescriptor);
};

#endif
