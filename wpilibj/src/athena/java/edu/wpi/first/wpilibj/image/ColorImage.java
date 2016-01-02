/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.image;

import com.ni.vision.NIVision;

/**
 * A class representing a color image.
 *
 * @author dtjones
 */
public abstract class ColorImage extends ImageBase {

  ColorImage(NIVision.ImageType type) throws NIVisionException {
    super(type);
  }

  ColorImage(ColorImage sourceImage) {
    super(sourceImage);
  }

  private BinaryImage threshold(NIVision.ColorMode colorMode, int low1, int high1, int low2,
      int high2, int low3, int high3) throws NIVisionException {
    BinaryImage res = new BinaryImage();
    NIVision.Range range1 = new NIVision.Range(low1, high1);
    NIVision.Range range2 = new NIVision.Range(low2, high2);
    NIVision.Range range3 = new NIVision.Range(low3, high3);
    NIVision.imaqColorThreshold(res.image, image, 1, colorMode, range1, range2, range3);
    res.free();
    range1.free();
    range2.free();
    range3.free();
    return res;
  }

  /**
   * Return a mask of the areas of the image that fall within the given ranges
   * for color values
   *
   * @param redLow The lower red limit.
   * @param redHigh The upper red limit.
   * @param greenLow The lower green limit.
   * @param greenHigh The upper green limit.
   * @param blueLow The lower blue limit.
   * @param blueHigh The upper blue limit.
   * @return A BinaryImage masking the areas which match the given thresholds.
   */
  public BinaryImage thresholdRGB(int redLow, int redHigh, int greenLow, int greenHigh,
      int blueLow, int blueHigh) throws NIVisionException {
    return threshold(NIVision.ColorMode.RGB, redLow, redHigh, greenLow, greenHigh, blueLow,
        blueHigh);
  }

  /**
   * Return a mask of the areas of the image that fall within the given ranges
   * for color values
   *
   * @param hueLow The lower hue limit.
   * @param hueHigh The upper hue limit.
   * @param saturationLow The lower saturation limit.
   * @param saturationHigh The upper saturation limit.
   * @param luminenceLow The lower luminence limit.
   * @param luminenceHigh The upper luminence limit.
   * @return A BinaryImage masking the areas which match the given thresholds.
   */
  public BinaryImage thresholdHSL(int hueLow, int hueHigh, int saturationLow, int saturationHigh,
      int luminenceLow, int luminenceHigh) throws NIVisionException {
    return threshold(NIVision.ColorMode.HSL, hueLow, hueHigh, saturationLow, saturationHigh,
        luminenceLow, luminenceHigh);
  }

  /**
   * Return a mask of the areas of the image that fall within the given ranges
   * for color values
   *
   * @param hueLow The lower hue limit.
   * @param hueHigh The upper hue limit.
   * @param saturationLow The lower saturation limit.
   * @param saturationHigh The upper saturation limit.
   * @param valueHigh The lower value limit.
   * @param valueLow The upper value limit.
   * @return A BinaryImage masking the areas which match the given thresholds.
   */
  public BinaryImage thresholdHSV(int hueLow, int hueHigh, int saturationLow, int saturationHigh,
      int valueLow, int valueHigh) throws NIVisionException {
    return threshold(NIVision.ColorMode.HSV, hueLow, hueHigh, saturationLow, saturationHigh,
        valueLow, valueHigh);
  }

  /**
   * Return a mask of the areas of the image that fall within the given ranges
   * for color values
   *
   * @param hueLow The lower hue limit.
   * @param hueHigh The upper hue limit.
   * @param saturationLow The lower saturation limit.
   * @param saturationHigh The upper saturation limit.
   * @param intansityLow The lower intensity limit.
   * @param intensityHigh The upper intensity limit.
   * @return A BinaryImage masking the areas which match the given thresholds.
   */
  public BinaryImage thresholdHSI(int hueLow, int hueHigh, int saturationLow, int saturationHigh,
      int intansityLow, int intensityHigh) throws NIVisionException {
    return threshold(NIVision.ColorMode.HSI, hueLow, hueHigh, saturationLow, saturationHigh,
        intansityLow, intensityHigh);
  }

  MonoImage extractFirstColorPlane(NIVision.ColorMode mode) throws NIVisionException {
    MonoImage result = new MonoImage();
    NIVision.imaqExtractColorPlanes(image, mode, result.image, null, null);
    result.free();
    return result;
  }

  MonoImage extractSecondColorPlane(NIVision.ColorMode mode) throws NIVisionException {
    MonoImage result = new MonoImage();
    NIVision.imaqExtractColorPlanes(image, mode, null, result.image, null);
    result.free();
    return result;
  }

  MonoImage extractThirdColorPlane(NIVision.ColorMode mode) throws NIVisionException {
    MonoImage result = new MonoImage();
    NIVision.imaqExtractColorPlanes(image, mode, null, null, result.image);
    result.free();
    return result;
  }

  /**
   * Get the red color plane from the image when represented in RGB color space.
   *
   * @return The red color plane from the image.
   */
  public MonoImage getRedPlane() throws NIVisionException {
    return extractFirstColorPlane(NIVision.ColorMode.RGB);
  }

  /**
   * Get the green color plane from the image when represented in RGB color
   * space.
   *
   * @return The green color plane from the image.
   */
  public MonoImage getGreenPlane() throws NIVisionException {
    return extractSecondColorPlane(NIVision.ColorMode.RGB);
  }

  /**
   * Get the blue color plane from the image when represented in RGB color
   * space.
   *
   * @return The blue color plane from the image.
   */
  public MonoImage getBluePlane() throws NIVisionException {
    return extractThirdColorPlane(NIVision.ColorMode.RGB);
  }

  /**
   * Get the hue color plane from the image when represented in HSL color space.
   *
   * @return The hue color plane from the image.
   */
  public MonoImage getHSLHuePlane() throws NIVisionException {
    return extractFirstColorPlane(NIVision.ColorMode.HSL);
  }

  /**
   * Get the hue color plane from the image when represented in HSV color space.
   *
   * @return The hue color plane from the image.
   */
  public MonoImage getHSVHuePlane() throws NIVisionException {
    return extractFirstColorPlane(NIVision.ColorMode.HSV);
  }

  /**
   * Get the hue color plane from the image when represented in HSI color space.
   *
   * @return The hue color plane from the image.
   */
  public MonoImage getHSIHuePlane() throws NIVisionException {
    return extractFirstColorPlane(NIVision.ColorMode.HSI);
  }

  /**
   * Get the saturation color plane from the image when represented in HSL color
   * space.
   *
   * @return The saturation color plane from the image.
   */
  public MonoImage getHSLSaturationPlane() throws NIVisionException {
    return extractSecondColorPlane(NIVision.ColorMode.HSL);
  }

  /**
   * Get the saturation color plane from the image when represented in HSV color
   * space.
   *
   * @return The saturation color plane from the image.
   */
  public MonoImage getHSVSaturationPlane() throws NIVisionException {
    return extractSecondColorPlane(NIVision.ColorMode.HSV);
  }

  /**
   * Get the saturation color plane from the image when represented in HSI color
   * space.
   *
   * @return The saturation color plane from the image.
   */
  public MonoImage getHSISaturationPlane() throws NIVisionException {
    return extractSecondColorPlane(NIVision.ColorMode.HSI);
  }

  /**
   * Get the luminance color plane from the image when represented in HSL color
   * space.
   *
   * @return The luminance color plane from the image.
   */
  public MonoImage getLuminancePlane() throws NIVisionException {
    return extractThirdColorPlane(NIVision.ColorMode.HSL);
  }

  /**
   * Get the value color plane from the image when represented in HSV color
   * space.
   *
   * @return The value color plane from the image.
   */
  public MonoImage getValuePlane() throws NIVisionException {
    return extractThirdColorPlane(NIVision.ColorMode.HSV);
  }

  /**
   * Get the intensity color plane from the image when represented in HSI color
   * space.
   *
   * @return The intensity color plane from the image.
   */
  public MonoImage getIntensityPlane() throws NIVisionException {
    return extractThirdColorPlane(NIVision.ColorMode.HSI);
  }

  ColorImage replaceFirstColorPlane(NIVision.ColorMode mode, MonoImage plane)
      throws NIVisionException {
    NIVision.imaqReplaceColorPlanes(image, image, mode, plane.image, null, null);
    return this;
  }

  ColorImage replaceSecondColorPlane(NIVision.ColorMode mode, MonoImage plane)
      throws NIVisionException {
    NIVision.imaqReplaceColorPlanes(image, image, mode, null, plane.image, null);
    return this;
  }

  ColorImage replaceThirdColorPlane(NIVision.ColorMode mode, MonoImage plane)
      throws NIVisionException {
    NIVision.imaqReplaceColorPlanes(image, image, mode, null, null, plane.image);
    return this;
  }

  /**
   * Set the red color plane from the image when represented in RGB color space.
   * This does not create a new image, but modifies this one instead. Create a
   * copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceRedPlane(MonoImage plane) throws NIVisionException {
    return replaceFirstColorPlane(NIVision.ColorMode.RGB, plane);
  }

  /**
   * Set the green color plane from the image when represented in RGB color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceGreenPlane(MonoImage plane) throws NIVisionException {
    return replaceSecondColorPlane(NIVision.ColorMode.RGB, plane);
  }

  /**
   * Set the blue color plane from the image when represented in RGB color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceBluePlane(MonoImage plane) throws NIVisionException {
    return replaceThirdColorPlane(NIVision.ColorMode.RGB, plane);
  }

  /**
   * Set the hue color plane from the image when represented in HSL color space.
   * This does not create a new image, but modifies this one instead. Create a
   * copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSLHuePlane(MonoImage plane) throws NIVisionException {
    return replaceFirstColorPlane(NIVision.ColorMode.HSL, plane);
  }

  /**
   * Set the hue color plane from the image when represented in HSV color space.
   * This does not create a new image, but modifies this one instead. Create a
   * copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSVHuePlane(MonoImage plane) throws NIVisionException {
    return replaceFirstColorPlane(NIVision.ColorMode.HSV, plane);
  }

  /**
   * Set the hue color plane from the image when represented in HSI color space.
   * This does not create a new image, but modifies this one instead. Create a
   * copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSIHuePlane(MonoImage plane) throws NIVisionException {
    return replaceFirstColorPlane(NIVision.ColorMode.HSI, plane);
  }

  /**
   * Set the saturation color plane from the image when represented in HSL color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSLSaturationPlane(MonoImage plane) throws NIVisionException {
    return replaceSecondColorPlane(NIVision.ColorMode.HSL, plane);
  }

  /**
   * Set the saturation color plane from the image when represented in HSV color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSVSaturationPlane(MonoImage plane) throws NIVisionException {
    return replaceSecondColorPlane(NIVision.ColorMode.HSV, plane);
  }

  /**
   * Set the saturation color plane from the image when represented in HSI color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceHSISaturationPlane(MonoImage plane) throws NIVisionException {
    return replaceSecondColorPlane(NIVision.ColorMode.HSI, plane);
  }

  /**
   * Set the luminance color plane from the image when represented in HSL color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceLuminancePlane(MonoImage plane) throws NIVisionException {
    return replaceThirdColorPlane(NIVision.ColorMode.HSL, plane);
  }

  /**
   * Set the value color plane from the image when represented in HSV color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceValuePlane(MonoImage plane) throws NIVisionException {
    return replaceThirdColorPlane(NIVision.ColorMode.HSV, plane);
  }

  /**
   * Set the intensity color plane from the image when represented in HSI color
   * space. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @param plane The MonoImage representing the new color plane.
   * @return The resulting image.
   */
  public ColorImage replaceIntensityPlane(MonoImage plane) throws NIVisionException {
    return replaceThirdColorPlane(NIVision.ColorMode.HSI, plane);
  }

  /**
   * Calculates the histogram of each plane of a color image and redistributes
   * pixel values across the desired range while maintaining pixel value
   * groupings. This does not create a new image, but modifies this one instead.
   * Create a copy before hand if you need to continue using the original.
   *
   * @return The modified image.
   */
  public ColorImage colorEqualize() throws NIVisionException {
    NIVision.imaqColorEqualize(image, image, 1);
    return this;
  }

  /**
   * Calculates the histogram of each plane of a color image and redistributes
   * pixel values across the desired range while maintaining pixel value
   * groupings for the Luminance plane only. This does not create a new image,
   * but modifies this one instead. Create a copy before hand if you need to
   * continue using the original.
   *
   * @return The modified image.
   */
  public ColorImage luminanceEqualize() throws NIVisionException {
    NIVision.imaqColorEqualize(image, image, 0);
    return this;
  }
}
