/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/ColorImage.h"

#include "WPIErrors.h"

ColorImage::ColorImage(ImageType type) : ImageBase(type)
{
}

ColorImage::~ColorImage()
{
}

/**
 * Perform a threshold operation on a ColorImage.
 * Perform a threshold operation on a ColorImage using the ColorMode supplied
 * as a parameter.
 * @param colorMode The type of colorspace this operation should be performed in
 * @returns a pointer to a binary image
 */
BinaryImage *ColorImage::ComputeThreshold(ColorMode colorMode,
		int low1, int high1,
		int low2, int high2,
		int low3, int high3)
{
	BinaryImage *result = new BinaryImage();
	Range range1 = {low1, high1},
		range2 = {low2, high2},
		range3 = {low3, high3};
	
	int success = imaqColorThreshold(result->GetImaqImage(), m_imaqImage, 1, colorMode, &range1, &range2, &range3);
	wpi_setImaqErrorWithContext(success, "ImaqThreshold error");
	return result;
}

/**
 * Perform a threshold in RGB space.
 * @param redLow Red low value
 * @param redHigh Red high value
 * @param greenLow Green low value
 * @param greenHigh Green high value
 * @param blueLow Blue low value
 * @param blueHigh Blue high value
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdRGB(int redLow, int redHigh, int greenLow, int greenHigh, int blueLow, int blueHigh)
{
	return ComputeThreshold(IMAQ_RGB, redLow, redHigh, greenLow, greenHigh, blueLow, blueHigh);
}

/**
 * Perform a threshold in RGB space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdRGB(Threshold &t)
{
	return ComputeThreshold(IMAQ_RGB, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSL space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param luminenceLow Low value for luminence
 * @param luminenceHigh High value for luminence
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSL(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int luminenceLow, int luminenceHigh)
{
	return ComputeThreshold(IMAQ_HSL, hueLow, hueHigh, saturationLow, saturationHigh, luminenceLow, luminenceHigh);
}

/**
 * Perform a threshold in HSL space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSL(Threshold &t)
{
	return ComputeThreshold(IMAQ_HSL, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSV space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param valueLow Low value
 * @param valueHigh High value
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSV(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int valueLow, int valueHigh)
{
	return ComputeThreshold(IMAQ_HSV, hueLow, hueHigh, saturationLow, saturationHigh, valueLow, valueHigh);
}

/**
 * Perform a threshold in HSV space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSV(Threshold &t)
{
	return ComputeThreshold(IMAQ_HSV, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSI space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param valueLow Low intensity
 * @param valueHigh High intensity
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSI(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int intensityLow, int intensityHigh)
{
	return ComputeThreshold(IMAQ_HSI, hueLow, hueHigh, saturationLow, saturationHigh, intensityLow, intensityHigh);
}

/**
 * Perform a threshold in HSI space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage *ColorImage::ThresholdHSI(Threshold &t)
{
	return ComputeThreshold(IMAQ_HSI, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Extract a color plane from the image
 * @param mode The ColorMode to use for the plane extraction
 * @param planeNumber Which plane is to be extracted
 * @returns A pointer to a MonoImage that represents the extracted plane.
 */
MonoImage *ColorImage::ExtractColorPlane(ColorMode mode, int planeNumber)
{
	MonoImage *result = new MonoImage();
	if (m_imaqImage == NULL)
		wpi_setWPIError(NullParameter);
	int success = imaqExtractColorPlanes(m_imaqImage, 
										 mode, 
										 (planeNumber == 1) ? result->GetImaqImage() : NULL, 
										 (planeNumber == 2) ? result->GetImaqImage() : NULL, 
										 (planeNumber == 3) ? result->GetImaqImage() : NULL);
	wpi_setImaqErrorWithContext(success, "Imaq ExtractColorPlanes failed");
	return result;
}

/*
 * Extract the first color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage *ColorImage::ExtractFirstColorPlane(ColorMode mode)
{
	return ExtractColorPlane(mode, 1);
}

/*
 * Extract the second color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage *ColorImage::ExtractSecondColorPlane(ColorMode mode)
{
	return ExtractColorPlane(mode, 2);
}

/*
 * Extract the third color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage *ColorImage::ExtractThirdColorPlane(ColorMode mode)
{
	return ExtractColorPlane(mode, 3);
}

/*
 * Extract the red plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetRedPlane()
{
	return ExtractFirstColorPlane(IMAQ_RGB);
}

/*
 * Extract the green plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetGreenPlane()
{
    return ExtractSecondColorPlane(IMAQ_RGB);
}

/*
 * Extract the blue plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetBluePlane()
{
    return ExtractThirdColorPlane(IMAQ_RGB);
}

/*
 * Extract the Hue plane from an HSL image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetHSLHuePlane()
{
	return ExtractFirstColorPlane(IMAQ_HSL);
}

/*
 * Extract the Hue plane from an HSV image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetHSVHuePlane()
{
	return ExtractFirstColorPlane(IMAQ_HSV);
}

/*
 * Extract the Hue plane from an HSI image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetHSIHuePlane()
{
	return ExtractFirstColorPlane(IMAQ_HSI);
}

/*
 * Extract the Luminance plane from an HSL image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetLuminancePlane()
{
	return ExtractThirdColorPlane(IMAQ_HSL);
}

/*
 * Extract the Value plane from an HSV image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetValuePlane()
{
	return ExtractThirdColorPlane(IMAQ_HSV);
}

/*
 * Extract the Intensity plane from an HSI image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage *ColorImage::GetIntensityPlane()
{
	return ExtractThirdColorPlane(IMAQ_HSI);
}

/**
 * Replace a plane in the ColorImage with a MonoImage
 * Replaces a single plane in the image with a MonoImage
 * @param mode The ColorMode in which to operate
 * @param plane The pointer to the replacement plane as a MonoImage
 * @param planeNumber The plane number (1, 2, 3) to replace
 */
void ColorImage::ReplacePlane(ColorMode mode, MonoImage *plane, int planeNumber) {
	int success = imaqReplaceColorPlanes(m_imaqImage, 
										 (const Image*) m_imaqImage, 
									     mode, 
									     (planeNumber == 1) ? plane->GetImaqImage() : NULL, 
									     (planeNumber == 2) ? plane->GetImaqImage() : NULL, 
									     (planeNumber == 3) ? plane->GetImaqImage() : NULL);
	wpi_setImaqErrorWithContext(success, "Imaq ReplaceColorPlanes failed");
}

/**
 * Replace the first color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceFirstColorPlane(ColorMode mode, MonoImage *plane)
{
	ReplacePlane(mode, plane, 1);
}

/**
 * Replace the second color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceSecondColorPlane(ColorMode mode, MonoImage *plane)
{
	ReplacePlane(mode, plane, 2);
}

/**
 * Replace the third color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceThirdColorPlane(ColorMode mode, MonoImage *plane)
{ 
	ReplacePlane(mode, plane, 3);
}

/**
 * Replace the red color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceRedPlane(MonoImage *plane)
{
	ReplaceFirstColorPlane(IMAQ_RGB, plane);
}

/**
 * Replace the green color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceGreenPlane(MonoImage *plane)
{
	ReplaceSecondColorPlane(IMAQ_RGB, plane);
}

/**
 * Replace the blue color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceBluePlane(MonoImage *plane)
{
	ReplaceThirdColorPlane(IMAQ_RGB, plane);
}


/**
 * Replace the Hue color plane in a HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSLHuePlane(MonoImage *plane)
{
	return ReplaceFirstColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Hue color plane in a HSV image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSVHuePlane(MonoImage *plane)
{
	return ReplaceFirstColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the first Hue plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSIHuePlane(MonoImage *plane)
{
	return ReplaceFirstColorPlane(IMAQ_HSI, plane);
}

/**
 * Replace the Saturation color plane in an HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSLSaturationPlane(MonoImage *plane)
{
	return ReplaceSecondColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Saturation color plane in a HSV image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSVSaturationPlane(MonoImage *plane)
{
	return ReplaceSecondColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the Saturation color plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceHSISaturationPlane(MonoImage *plane)
{
	return ReplaceSecondColorPlane(IMAQ_HSI, plane);
}

/**
 * Replace the Luminance color plane in an HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceLuminancePlane(MonoImage *plane)
{
	return ReplaceThirdColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Value color plane in an HSV with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceValuePlane(MonoImage *plane)
{
	return ReplaceThirdColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the Intensity color plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::ReplaceIntensityPlane(MonoImage *plane)
{
	return ReplaceThirdColorPlane(IMAQ_HSI, plane);
}

//TODO: frcColorEqualize(Image* dest, const Image* source, int colorEqualization) needs to be modified
//The colorEqualization parameter is discarded and is set to TRUE in the call to imaqColorEqualize.
void ColorImage::Equalize(bool allPlanes)
{
	// Note that this call uses NI-defined TRUE and FALSE
	int success = imaqColorEqualize(m_imaqImage, (const Image*) m_imaqImage, (allPlanes) ? TRUE : FALSE);
	wpi_setImaqErrorWithContext(success, "Imaq ColorEqualize error");
}

void ColorImage::ColorEqualize()
{
	Equalize(true);
}

void ColorImage::LuminanceEqualize()
{
	Equalize(false);
}
