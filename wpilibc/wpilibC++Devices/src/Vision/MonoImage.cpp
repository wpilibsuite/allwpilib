/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Vision/MonoImage.h"
#include "nivision.h"

using namespace std;

MonoImage::MonoImage() : ImageBase(IMAQ_IMAGE_U8) {}

/**
 * Look for ellipses in an image.
 * Given some input parameters, look for any number of ellipses in an image.
 * @param ellipseDescriptor Ellipse descriptor
 * @param curveOptions Curve options
 * @param shapeDetectionOptions Shape detection options
 * @param roi Region of Interest
 * @returns a vector of EllipseMatch structures (0 length vector on no match)
 */
vector<EllipseMatch> *MonoImage::DetectEllipses(
    EllipseDescriptor *ellipseDescriptor, CurveOptions *curveOptions,
    ShapeDetectionOptions *shapeDetectionOptions, ROI *roi) {
  int numberOfMatches;
  EllipseMatch *e =
      imaqDetectEllipses(m_imaqImage, ellipseDescriptor, curveOptions,
                         shapeDetectionOptions, roi, &numberOfMatches);
  auto ellipses = new vector<EllipseMatch>;
  if (e == nullptr) {
    return ellipses;
  }
  for (int i = 0; i < numberOfMatches; i++) {
    ellipses->push_back(e[i]);
  }
  imaqDispose(e);
  return ellipses;
}

vector<EllipseMatch> *MonoImage::DetectEllipses(
    EllipseDescriptor *ellipseDescriptor) {
  vector<EllipseMatch> *ellipses =
      DetectEllipses(ellipseDescriptor, nullptr, nullptr, nullptr);
  return ellipses;
}
