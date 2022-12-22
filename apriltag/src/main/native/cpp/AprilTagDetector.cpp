// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagDetector.h"

#include <cmath>

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"
#include "tag16h5.h"
#include "tag25h9.h"
#include "tag36h11.h"
#include "tagCircle21h7.h"
#include "tagCircle49h12.h"
#include "tagCustom48h12.h"
#include "tagStandard41h12.h"
#include "tagStandard52h13.h"

using namespace frc;

AprilTagDetector::Results::Results(void* impl, const private_init&)
    : span{reinterpret_cast<AprilTagDetection**>(
               static_cast<zarray_t*>(impl)->data),
           static_cast<size_t>(static_cast<zarray_t*>(impl)->size)},
      m_impl{impl} {}

AprilTagDetector::Results& AprilTagDetector::Results::operator=(Results&& rhs) {
  Destroy();
  m_impl = rhs.m_impl;
  rhs.m_impl = nullptr;
  return *this;
}

void AprilTagDetector::Results::Destroy() {
  if (m_impl) {
    apriltag_detections_destroy(static_cast<zarray_t*>(m_impl));
  }
}

AprilTagDetector::AprilTagDetector() : m_impl{apriltag_detector_create()} {
  // apriltag_detector_create does not appropriately initialize qtp.critical_rad
  static_cast<apriltag_detector_t*>(m_impl)->qtp.critical_rad = 10 * M_PI / 180;
}

AprilTagDetector& AprilTagDetector::operator=(AprilTagDetector&& rhs) {
  Destroy();
  m_impl = rhs.m_impl;
  rhs.m_impl = nullptr;
  m_families = std::move(rhs.m_families);
  rhs.m_families.clear();
  return *this;
}

void AprilTagDetector::SetNumThreads(int val) {
  static_cast<apriltag_detector_t*>(m_impl)->nthreads = val;
}

int AprilTagDetector::GetNumThreads() const {
  return static_cast<apriltag_detector_t*>(m_impl)->nthreads;
}

void AprilTagDetector::SetQuadDecimate(float val) {
  static_cast<apriltag_detector_t*>(m_impl)->quad_decimate = val;
}

float AprilTagDetector::GetQuadDecimate() const {
  return static_cast<apriltag_detector_t*>(m_impl)->quad_decimate;
}

void AprilTagDetector::SetQuadSigma(float val) {
  static_cast<apriltag_detector_t*>(m_impl)->quad_sigma = val;
}

float AprilTagDetector::GetQuadSigma() const {
  return static_cast<apriltag_detector_t*>(m_impl)->quad_sigma;
}

void AprilTagDetector::SetRefineEdges(bool val) {
  static_cast<apriltag_detector_t*>(m_impl)->refine_edges = val;
}

bool AprilTagDetector::GetRefineEdges() const {
  return static_cast<apriltag_detector_t*>(m_impl)->refine_edges;
}

void AprilTagDetector::SetDecodeSharpening(double val) {
  static_cast<apriltag_detector_t*>(m_impl)->decode_sharpening = val;
}

double AprilTagDetector::GetDecodeSharpening() const {
  return static_cast<apriltag_detector_t*>(m_impl)->decode_sharpening;
}

void AprilTagDetector::SetDebug(bool val) {
  static_cast<apriltag_detector_t*>(m_impl)->debug = val;
}

bool AprilTagDetector::GetDebug() const {
  return static_cast<apriltag_detector_t*>(m_impl)->debug;
}

void AprilTagDetector::SetQuadMinClusterPixels(int val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.min_cluster_pixels = val;
}

int AprilTagDetector::GetQuadMinClusterPixels() const {
  return static_cast<apriltag_detector_t*>(m_impl)->qtp.min_cluster_pixels;
}

void AprilTagDetector::SetQuadMaxNumMaxima(int val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.max_nmaxima = val;
}

int AprilTagDetector::GetQuadMaxNumMaxima() const {
  return static_cast<apriltag_detector_t*>(m_impl)->qtp.max_nmaxima;
}

void AprilTagDetector::SetQuadCriticalAngle(units::radian_t val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.critical_rad = val.value();
  static_cast<apriltag_detector_t*>(m_impl)->qtp.cos_critical_rad =
      std::cos(val.value());
}

units::radian_t AprilTagDetector::GetQuadCriticalAngle() const {
  return units::radian_t{
      static_cast<apriltag_detector_t*>(m_impl)->qtp.critical_rad};
}

void AprilTagDetector::SetQuadMaxLineFitMSE(float val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.max_line_fit_mse = val;
}

float AprilTagDetector::GetQuadMaxLineFitMSE() const {
  return static_cast<apriltag_detector_t*>(m_impl)->qtp.max_line_fit_mse;
}

void AprilTagDetector::SetQuadMinWhiteBlackDiff(int val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.min_white_black_diff = val;
}

int AprilTagDetector::GetQuadMinWhiteBlackDiff() const {
  return static_cast<apriltag_detector_t*>(m_impl)->qtp.min_white_black_diff;
}

void AprilTagDetector::SetQuadDeglitch(bool val) {
  static_cast<apriltag_detector_t*>(m_impl)->qtp.deglitch = val;
}

bool AprilTagDetector::GetQuadDeglitch() const {
  return static_cast<apriltag_detector_t*>(m_impl)->qtp.deglitch;
}

bool AprilTagDetector::AddFamily(std::string_view fam, int bitsCorrected) {
  auto& data = m_families[fam];
  if (data) {
    return true;  // already detecting
  }
  // create the family
  if (fam == "tag16h5") {
    data = tag16h5_create();
  } else if (fam == "tag25h9") {
    data = tag25h9_create();
  } else if (fam == "tag36h11") {
    data = tag36h11_create();
  } else if (fam == "tagCircle21h7") {
    data = tagCircle21h7_create();
  } else if (fam == "tagCircle49h12") {
    data = tagCircle49h12_create();
  } else if (fam == "tagStandard41h12") {
    data = tagStandard41h12_create();
  } else if (fam == "tagStandard52h13") {
    data = tagStandard52h13_create();
  } else if (fam == "tagCustom48h12") {
    data = tagCustom48h12_create();
  }
  if (!data) {
    m_families.erase(fam);  // don't keep null value
    return false;           // can't add
  }
  apriltag_detector_add_family_bits(static_cast<apriltag_detector_t*>(m_impl),
                                    static_cast<apriltag_family_t*>(data),
                                    bitsCorrected);
  return true;
}

void AprilTagDetector::RemoveFamily(std::string_view fam) {
  auto it = m_families.find(fam);
  if (it != m_families.end()) {
    apriltag_detector_remove_family(
        static_cast<apriltag_detector_t*>(m_impl),
        static_cast<apriltag_family_t*>(it->second));
    DestroyFamily(it->getKey(), it->second);
    m_families.erase(it);
  }
}

void AprilTagDetector::ClearFamilies() {
  apriltag_detector_clear_families(static_cast<apriltag_detector_t*>(m_impl));
  DestroyFamilies();
  m_families.clear();
}

AprilTagDetector::Results AprilTagDetector::Detect(int width, int height,
                                                   int stride, uint8_t* buf) {
  image_u8_t img{width, height, stride, buf};
  return {
      apriltag_detector_detect(static_cast<apriltag_detector_t*>(m_impl), &img),
      Results::private_init{}};
}

void AprilTagDetector::Destroy() {
  if (m_impl) {
    apriltag_detector_destroy(static_cast<apriltag_detector_t*>(m_impl));
  }
  DestroyFamilies();
}

void AprilTagDetector::DestroyFamilies() {
  for (auto&& entry : m_families) {
    DestroyFamily(entry.getKey(), entry.second);
  }
}

void AprilTagDetector::DestroyFamily(std::string_view name, void* data) {
  auto fam = static_cast<apriltag_family_t*>(data);
  if (name == "tag16h5") {
    tag16h5_destroy(fam);
  } else if (name == "tag25h9") {
    tag25h9_destroy(fam);
  } else if (name == "tag36h11") {
    tag36h11_destroy(fam);
  } else if (name == "tagCircle21h7") {
    tagCircle21h7_destroy(fam);
  } else if (name == "tagCircle49h12") {
    tagCircle49h12_destroy(fam);
  } else if (name == "tagStandard41h12") {
    tagStandard41h12_destroy(fam);
  } else if (name == "tagStandard52h13") {
    tagStandard52h13_destroy(fam);
  } else if (name == "tagCustom48h12") {
    tagCustom48h12_destroy(fam);
  }
}
