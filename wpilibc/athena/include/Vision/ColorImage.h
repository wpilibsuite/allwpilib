/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "BinaryImage.h"
#include "ImageBase.h"
#include "Threshold.h"

class ColorImage : public ImageBase {
 public:
  ColorImage(ImageType type);
  virtual ~ColorImage() = default;
  BinaryImage* ThresholdRGB(int32_t redLow, int32_t redHigh, int32_t greenLow,
                            int32_t greenHigh, int32_t blueLow,
                            int32_t blueHigh);
  BinaryImage* ThresholdHSL(int32_t hueLow, int32_t hueHigh,
                            int32_t saturationLow, int32_t saturationHigh,
                            int32_t luminenceLow, int32_t luminenceHigh);
  BinaryImage* ThresholdHSV(int32_t hueLow, int32_t hueHigh,
                            int32_t saturationLow, int32_t saturationHigh,
                            int32_t valueHigh, int32_t valueLow);
  BinaryImage* ThresholdHSI(int32_t hueLow, int32_t hueHigh,
                            int32_t saturationLow, int32_t saturationHigh,
                            int32_t intensityLow, int32_t intensityHigh);
  BinaryImage* ThresholdRGB(Threshold& threshold);
  BinaryImage* ThresholdHSL(Threshold& threshold);
  BinaryImage* ThresholdHSV(Threshold& threshold);
  BinaryImage* ThresholdHSI(Threshold& threshold);
  MonoImage* GetRedPlane();
  MonoImage* GetGreenPlane();
  MonoImage* GetBluePlane();
  MonoImage* GetHSLHuePlane();
  MonoImage* GetHSVHuePlane();
  MonoImage* GetHSIHuePlane();
  MonoImage* GetHSLSaturationPlane();
  MonoImage* GetHSVSaturationPlane();
  MonoImage* GetHSISaturationPlane();
  MonoImage* GetLuminancePlane();
  MonoImage* GetValuePlane();
  MonoImage* GetIntensityPlane();
  void ReplaceRedPlane(MonoImage* plane);
  void ReplaceGreenPlane(MonoImage* plane);
  void ReplaceBluePlane(MonoImage* plane);
  void ReplaceHSLHuePlane(MonoImage* plane);
  void ReplaceHSVHuePlane(MonoImage* plane);
  void ReplaceHSIHuePlane(MonoImage* plane);
  void ReplaceHSLSaturationPlane(MonoImage* plane);
  void ReplaceHSVSaturationPlane(MonoImage* plane);
  void ReplaceHSISaturationPlane(MonoImage* plane);
  void ReplaceLuminancePlane(MonoImage* plane);
  void ReplaceValuePlane(MonoImage* plane);
  void ReplaceIntensityPlane(MonoImage* plane);
  void ColorEqualize();
  void LuminanceEqualize();

 private:
  BinaryImage* ComputeThreshold(ColorMode colorMode, int32_t low1,
                                int32_t high1, int32_t low2, int32_t high2,
                                int32_t low3, int32_t high3);
  void Equalize(bool allPlanes);
  MonoImage* ExtractColorPlane(ColorMode mode, int32_t planeNumber);
  MonoImage* ExtractFirstColorPlane(ColorMode mode);
  MonoImage* ExtractSecondColorPlane(ColorMode mode);
  MonoImage* ExtractThirdColorPlane(ColorMode mode);
  void ReplacePlane(ColorMode mode, MonoImage* plane, int32_t planeNumber);
  void ReplaceFirstColorPlane(ColorMode mode, MonoImage* plane);
  void ReplaceSecondColorPlane(ColorMode mode, MonoImage* plane);
  void ReplaceThirdColorPlane(ColorMode mode, MonoImage* plane);
};
