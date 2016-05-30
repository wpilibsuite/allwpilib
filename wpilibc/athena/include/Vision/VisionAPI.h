/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "nivision.h"

/* Constants */

#define DEFAULT_BORDER_SIZE 3            // VisionAPI.frcCreateImage
#define DEFAULT_SATURATION_THRESHOLD 40  // TrackAPI.FindColor

/* Forward Declare Data Structures */
typedef struct FindEdgeOptions_struct FindEdgeOptions;
typedef struct CircularEdgeReport_struct CircularEdgeReport;

/* Data Structures */

/** frcParticleAnalysis returns this structure */
typedef struct ParticleAnalysisReport_struct {
  int32_t imageHeight;
  int32_t imageWidth;
  double imageTimestamp;
  int32_t particleIndex;  // the particle index analyzed
  /* X-coordinate of the point representing the average position of the
   * total particle mass, assuming every point in the particle has a constant
   * density
   */
  int32_t center_mass_x;  // MeasurementType: IMAQ_MT_CENTER_OF_MASS_X
  /* Y-coordinate of the point representing the average position of the
   * total particle mass, assuming every point in the particle has a constant
   * density
   */
  int32_t center_mass_y;            // MeasurementType: IMAQ_MT_CENTER_OF_MASS_Y
  double center_mass_x_normalized;  // Center of mass x value normalized to -1.0
                                    // to +1.0 range
  double center_mass_y_normalized;  // Center of mass y value normalized to -1.0
                                    // to +1.0 range
  /* Area of the particle */
  double particleArea;  // MeasurementType: IMAQ_MT_AREA
  /* Bounding Rectangle */
  Rect boundingRect;  // left/top/width/height
  /* Percentage of the particle Area covering the Image Area. */
  double particleToImagePercent;  // MeasurementType: IMAQ_MT_AREA_BY_IMAGE_AREA
  /* Percentage of the particle Area in relation to its Particle and Holes Area
   */
  double particleQuality;  // MeasurementType:
                           // IMAQ_MT_AREA_BY_PARTICLE_AND_HOLES_AREA
} ParticleAnalysisReport;

/** Tracking functions return this structure */
typedef struct ColorReport_struct {
  int32_t numberParticlesFound;   // Number of particles found for this color
  int32_t largestParticleNumber;  // The particle index of the largest particle
  /* Color information */
  float particleHueMax;   // HistogramReport: hue max
  float particleHueMin;   // HistogramReport: hue max
  float particleHueMean;  // HistogramReport: hue mean
  float particleSatMax;   // HistogramReport: saturation max
  float particleSatMin;   // HistogramReport: saturation max
  float particleSatMean;  // HistogramReport: saturation mean
  float particleLumMax;   // HistogramReport: luminance max
  float particleLumMin;   // HistogramReport: luminance  max
  float particleLumMean;  // HistogramReport: luminance mean
} ColorReport;

/* Image Management functions */

/* Create: calls imaqCreateImage. Border size is set to some default value */
Image* frcCreateImage(ImageType type);

/* Dispose: calls imaqDispose */
int32_t frcDispose(void* object);
int32_t frcDispose(const char* filename, ...);

/* Copy: calls imaqDuplicateImage */
int32_t frcCopyImage(Image* dest, const Image* source);

/* Image Extraction: Crop: calls imaqScale */
int32_t frcCrop(Image* dest, const Image* source, Rect rect);

/* Image Extraction: Scale: calls imaqScale.  Scales entire image */
int32_t frcScale(Image* dest, const Image* source, int32_t xScale,
                 int32_t yScale, ScalingMode scaleMode);

/* Read Image : calls imaqReadFile */
int32_t frcReadImage(Image* image, const char* fileName);
/* Write Image : calls imaqWriteFile */
int32_t frcWriteImage(const Image* image, const char* fileName);

/* Measure Intensity functions */

/* Histogram: calls imaqHistogram */
HistogramReport* frcHistogram(const Image* image, int32_t numClasses, float min,
                              float max, Rect rect);
/* Color Histogram: calls imaqColorHistogram2 */
ColorHistogramReport* frcColorHistogram(const Image* image, int32_t numClasses,
                                        ColorMode mode, Image* mask);

/* Get Pixel Value: calls imaqGetPixel */
int32_t frcGetPixelValue(const Image* image, Point pixel, PixelValue* value);

/* Particle Analysis functions */

/* Particle Filter: calls imaqParticleFilter3 */
int32_t frcParticleFilter(Image* dest, Image* source,
                          const ParticleFilterCriteria2* criteria,
                          int32_t criteriaCount,
                          const ParticleFilterOptions* options, Rect rect,
                          int32_t* numParticles);
int32_t frcParticleFilter(Image* dest, Image* source,
                          const ParticleFilterCriteria2* criteria,
                          int32_t criteriaCount,
                          const ParticleFilterOptions* options,
                          int32_t* numParticles);
/* Morphology: calls imaqMorphology */
int32_t frcMorphology(Image* dest, Image* source, MorphologyMethod method);
int32_t frcMorphology(Image* dest, Image* source, MorphologyMethod method,
                      const StructuringElement* structuringElement);
/* Reject Border: calls imaqRejectBorder */
int32_t frcRejectBorder(Image* dest, Image* source);
int32_t frcRejectBorder(Image* dest, Image* source, int32_t connectivity8);
/* Count Particles: calls imaqCountParticles */
int32_t frcCountParticles(Image* image, int32_t* numParticles);
/* Particle Analysis Report: calls imaqMeasureParticle */
int32_t frcParticleAnalysis(Image* image, int32_t particleNumber,
                            ParticleAnalysisReport* par);

/* Image Enhancement functions */

/* Equalize: calls imaqEqualize */
int32_t frcEqualize(Image* dest, const Image* source, float min, float max);
int32_t frcEqualize(Image* dest, const Image* source, float min, float max,
                    const Image* mask);

/* Color Equalize: calls imaqColorEqualize */
int32_t frcColorEqualize(Image* dest, const Image* source);
int32_t frcColorEqualize(Image* dest, const Image* source,
                         int32_t colorEqualization);

/* Image Thresholding & Conversion functions */

/* Smart Threshold: calls imaqLocalThreshold */
int32_t frcSmartThreshold(Image* dest, const Image* source,
                          uint32_t windowWidth, uint32_t windowHeight,
                          LocalThresholdMethod method, double deviationWeight,
                          ObjectType type);
int32_t frcSmartThreshold(Image* dest, const Image* source,
                          uint32_t windowWidth, uint32_t windowHeight,
                          LocalThresholdMethod method, double deviationWeight,
                          ObjectType type, float replaceValue);

/* Simple Threshold: calls imaqThreshold */
int32_t frcSimpleThreshold(Image* dest, const Image* source, float rangeMin,
                           float rangeMax, float newValue);
int32_t frcSimpleThreshold(Image* dest, const Image* source, float rangeMin,
                           float rangeMax);

/* Color/Hue Threshold: calls imaqColorThreshold */
int32_t frcColorThreshold(Image* dest, const Image* source, ColorMode mode,
                          const Range* plane1Range, const Range* plane2Range,
                          const Range* plane3Range);
int32_t frcColorThreshold(Image* dest, const Image* source,
                          int32_t replaceValue, ColorMode mode,
                          const Range* plane1Range, const Range* plane2Range,
                          const Range* plane3Range);
int32_t frcHueThreshold(Image* dest, const Image* source,
                        const Range* hueRange);
int32_t frcHueThreshold(Image* dest, const Image* source, const Range* hueRange,
                        int32_t minSaturation);

/* Extract ColorHue Plane: calls imaqExtractColorPlanes */
int32_t frcExtractColorPlanes(const Image* image, ColorMode mode, Image* plane1,
                              Image* plane2, Image* plane3);
int32_t frcExtractHuePlane(const Image* image, Image* huePlane);
int32_t frcExtractHuePlane(const Image* image, Image* huePlane,
                           int32_t minSaturation);
