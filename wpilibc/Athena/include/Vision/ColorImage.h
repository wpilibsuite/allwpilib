/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ImageBase.h"
#include "BinaryImage.h"
#include "Threshold.h"

class ColorImage : public ImageBase {
 public:
  ColorImage(ImageType type);
  virtual ~ColorImage() = default;
  BinaryImage *ThresholdRGB(int redLow, int redHigh, int greenLow,
                            int greenHigh, int blueLow, int blueHigh);
  BinaryImage *ThresholdHSL(int hueLow, int hueHigh, int saturationLow,
                            int saturationHigh, int luminenceLow,
                            int luminenceHigh);
  BinaryImage *ThresholdHSV(int hueLow, int hueHigh, int saturationLow,
                            int saturationHigh, int valueHigh, int valueLow);
  BinaryImage *ThresholdHSI(int hueLow, int hueHigh, int saturationLow,
                            int saturationHigh, int intensityLow,
                            int intensityHigh);
  BinaryImage *ThresholdRGB(Threshold &threshold);
  BinaryImage *ThresholdHSL(Threshold &threshold);
  BinaryImage *ThresholdHSV(Threshold &threshold);
  BinaryImage *ThresholdHSI(Threshold &threshold);
  MonoImage *GetRedPlane();
  MonoImage *GetGreenPlane();
  MonoImage *GetBluePlane();
  MonoImage *GetHSLHuePlane();
  MonoImage *GetHSVHuePlane();
  MonoImage *GetHSIHuePlane();
  MonoImage *GetHSLSaturationPlane();
  MonoImage *GetHSVSaturationPlane();
  MonoImage *GetHSISaturationPlane();
  MonoImage *GetLuminancePlane();
  MonoImage *GetValuePlane();
  MonoImage *GetIntensityPlane();
  void ReplaceRedPlane(MonoImage *plane);
  void ReplaceGreenPlane(MonoImage *plane);
  void ReplaceBluePlane(MonoImage *plane);
  void ReplaceHSLHuePlane(MonoImage *plane);
  void ReplaceHSVHuePlane(MonoImage *plane);
  void ReplaceHSIHuePlane(MonoImage *plane);
  void ReplaceHSLSaturationPlane(MonoImage *plane);
  void ReplaceHSVSaturationPlane(MonoImage *plane);
  void ReplaceHSISaturationPlane(MonoImage *plane);
  void ReplaceLuminancePlane(MonoImage *plane);
  void ReplaceValuePlane(MonoImage *plane);
  void ReplaceIntensityPlane(MonoImage *plane);
  void ColorEqualize();
  void LuminanceEqualize();

 private:
  BinaryImage *ComputeThreshold(ColorMode colorMode, int low1, int high1,
                                int low2, int high2, int low3, int high3);
  void Equalize(bool allPlanes);
  MonoImage *ExtractColorPlane(ColorMode mode, int planeNumber);
  MonoImage *ExtractFirstColorPlane(ColorMode mode);
  MonoImage *ExtractSecondColorPlane(ColorMode mode);
  MonoImage *ExtractThirdColorPlane(ColorMode mode);
  void ReplacePlane(ColorMode mode, MonoImage *plane, int planeNumber);
  void ReplaceFirstColorPlane(ColorMode mode, MonoImage *plane);
  void ReplaceSecondColorPlane(ColorMode mode, MonoImage *plane);
  void ReplaceThirdColorPlane(ColorMode mode, MonoImage *plane);
};
