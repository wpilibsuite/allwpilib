/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Vision/RGBImage.h"

/**
 * Create a new image that uses Red, Green, and Blue planes.
 */
RGBImage::RGBImage() : ColorImage(IMAQ_IMAGE_RGB) {}

/**
 * Create a new image by loading a file.
 * @param fileName The path of the file to load.
 */
RGBImage::RGBImage(const char *fileName) : ColorImage(IMAQ_IMAGE_RGB) {
  int success = imaqReadFile(m_imaqImage, fileName, nullptr, nullptr);
  wpi_setImaqErrorWithContext(success, "Imaq ReadFile error");
}
