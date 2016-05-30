/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <vector>

#include "MonoImage.h"
/**
 * Included for ParticleAnalysisReport definition
 * TODO: Eliminate this dependency!
 */
#include "Vision/VisionAPI.h"

class BinaryImage : public MonoImage {
 public:
  virtual ~BinaryImage() = default;
  int32_t GetNumberParticles();
  ParticleAnalysisReport GetParticleAnalysisReport(int32_t particleNumber);
  void GetParticleAnalysisReport(int32_t particleNumber,
                                 ParticleAnalysisReport* par);
  std::vector<ParticleAnalysisReport>* GetOrderedParticleAnalysisReports();
  BinaryImage* RemoveSmallObjects(bool connectivity8, int32_t erosions);
  BinaryImage* RemoveLargeObjects(bool connectivity8, int32_t erosions);
  BinaryImage* ConvexHull(bool connectivity8);
  BinaryImage* ParticleFilter(ParticleFilterCriteria2* criteria,
                              int32_t criteriaCount);
  virtual void Write(const char* fileName);

 private:
  bool ParticleMeasurement(int32_t particleNumber,
                           MeasurementType whatToMeasure, int32_t* result);
  bool ParticleMeasurement(int32_t particleNumber,
                           MeasurementType whatToMeasure, double* result);
  static double NormalizeFromRange(double position, int32_t range);
  static bool CompareParticleSizes(ParticleAnalysisReport particle1,
                                   ParticleAnalysisReport particle2);
};
