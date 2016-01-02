/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Vision/HSLImage.h"

/**
 * Create a new image that uses the Hue, Saturation, and Luminance planes.
 */
HSLImage::HSLImage() : ColorImage(IMAQ_IMAGE_HSL) {}

/**
 * Create a new image by loading a file.
 * @param fileName The path of the file to load.
 */
HSLImage::HSLImage(const char *fileName) : ColorImage(IMAQ_IMAGE_HSL) {
  int success = imaqReadFile(m_imaqImage, fileName, nullptr, nullptr);
  wpi_setImaqErrorWithContext(success, "Imaq ReadFile error");
}
