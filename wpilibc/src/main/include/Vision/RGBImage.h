/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __RGB_IMAGE_H__
#define __RGB_IMAGE_H__

#include "ColorImage.h"

/**
 * A color image represented in RGB color space at 3 bytes per pixel.
 */
class RGBImage : public ColorImage
{
public:
    RGBImage();
    RGBImage(const char *fileName);
    virtual ~RGBImage();
};

#endif
