/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ColorImage.h"

/**
 * A color image represented in HSL color space at 3 bytes per pixel.
 */
class HSLImage : public ColorImage
{
public:
    HSLImage();
    HSLImage(const char *fileName);
    virtual ~HSLImage();
};


