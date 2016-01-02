/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.ni.vision.NIVision;
import com.ni.vision.NIVision.Image;

/**
 * Class representing a generic image.
 *$
 * @author dtjones
 */
public abstract class ImageBase {

  /**
   * Pointer to the image memory
   */
  public final Image image;
  static final int DEFAULT_BORDER_SIZE = 3;

  ImageBase(NIVision.ImageType type) throws NIVisionException {
    image = NIVision.imaqCreateImage(type, DEFAULT_BORDER_SIZE);
  }

  /**
   * Creates a new image pointing to the same data as the source image. The
   * imgae data is not copied, it is just referenced by both objects. Freeing
   * one will free both.
   *$
   * @param sourceImage The image to reference
   */
  ImageBase(ImageBase sourceImage) {
    image = sourceImage.image;
  }

  /**
   * Write the image to a file.
   *
   * Supported extensions: .aipd or .apd AIPD .bmp BMP .jpg or .jpeg JPEG .jp2
   * JPEG2000 .png PNG .tif or .tiff TIFF
   *
   * @param fileName The path to write the image to.
   */
  public void write(String fileName) throws NIVisionException {
    NIVision.RGBValue value = new NIVision.RGBValue();
    NIVision.imaqWriteFile(image, fileName, value);
    value.free();
  }

  /**
   * Release the memory associated with an image.
   */
  public void free() throws NIVisionException {
    image.free();
  }

  /**
   * Get the height of the image in pixels.
   *$
   * @return The height of the image.
   */
  public int getHeight() throws NIVisionException {
    return NIVision.imaqGetImageSize(image).height;
  }

  /**
   * Get the width of the image in pixels.
   *$
   * @return The width of the image.
   */
  public int getWidth() throws NIVisionException {
    return NIVision.imaqGetImageSize(image).width;
  }
}
