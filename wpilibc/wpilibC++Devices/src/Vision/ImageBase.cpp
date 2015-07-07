/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/ImageBase.h"
#include "nivision.h"

/**
 * Create a new instance of an ImageBase.
 * Imagebase is the base of all the other image classes. The constructor
 * creates any type of image and stores the pointer to it in the class.
 * @param type The type of image to create
 */
ImageBase::ImageBase(ImageType type) {
  m_imaqImage = imaqCreateImage(type, DEFAULT_BORDER_SIZE);
}

/**
 * Frees memory associated with an ImageBase.
 * Destructor frees the imaq image allocated with the class.
 */
ImageBase::~ImageBase() {
  if (m_imaqImage) imaqDispose(m_imaqImage);
}

/**
 * Writes an image to a file with the given filename.
 * Write the image to a file in the flash on the cRIO.
 * @param fileName The name of the file to write
 */
void ImageBase::Write(const char *fileName) {
  int success = imaqWriteFile(m_imaqImage, fileName, nullptr);
  wpi_setImaqErrorWithContext(success, "Imaq Image writeFile error");
}

/**
 * Gets the height of an image.
 * @return The height of the image in pixels.
 */
int ImageBase::GetHeight() {
  int height;
  imaqGetImageSize(m_imaqImage, nullptr, &height);
  return height;
}

/**
 * Gets the width of an image.
 * @return The width of the image in pixels.
 */
int ImageBase::GetWidth() {
  int width;
  imaqGetImageSize(m_imaqImage, &width, nullptr);
  return width;
}

/**
 * Access the internal IMAQ Image data structure.
 *
 * @return A pointer to the internal IMAQ Image data structure.
 */
Image *ImageBase::GetImaqImage() { return m_imaqImage; }
