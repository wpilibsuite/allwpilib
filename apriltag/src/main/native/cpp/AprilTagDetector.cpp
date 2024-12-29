// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagDetector.h"

#include <cmath>
#include <utility>

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"
#include "tag16h5.h"
#include "tag36h11.h"

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
  SetQuadThresholdParameters({});
}

AprilTagDetector& AprilTagDetector::operator=(AprilTagDetector&& rhs) {
  Destroy();
  m_impl = rhs.m_impl;
  rhs.m_impl = nullptr;
  m_families = std::move(rhs.m_families);
  rhs.m_families.clear();
  m_qtpCriticalAngle = rhs.m_qtpCriticalAngle;
  return *this;
}

void AprilTagDetector::SetConfig(const Config& config) {
  auto& impl = *static_cast<apriltag_detector_t*>(m_impl);
  impl.nthreads = config.numThreads;
  impl.quad_decimate = config.quadDecimate;
  impl.quad_sigma = config.quadSigma;
  impl.refine_edges = config.refineEdges;
  impl.decode_sharpening = config.decodeSharpening;
  impl.debug = config.debug;
}

AprilTagDetector::Config AprilTagDetector::GetConfig() const {
  auto& impl = *static_cast<apriltag_detector_t*>(m_impl);
  return {
      .numThreads = impl.nthreads,
      .quadDecimate = impl.quad_decimate,
      .quadSigma = impl.quad_sigma,
      .refineEdges = impl.refine_edges,
      .decodeSharpening = impl.decode_sharpening,
      .debug = impl.debug,
  };
}

void AprilTagDetector::SetQuadThresholdParameters(
    const QuadThresholdParameters& params) {
  auto& qtp = static_cast<apriltag_detector_t*>(m_impl)->qtp;
  qtp.min_cluster_pixels = params.minClusterPixels;
  qtp.max_nmaxima = params.maxNumMaxima;
  qtp.critical_rad = params.criticalAngle.value();
  qtp.cos_critical_rad = std::cos(params.criticalAngle.value());
  qtp.max_line_fit_mse = params.maxLineFitMSE;
  qtp.min_white_black_diff = params.minWhiteBlackDiff;
  qtp.deglitch = params.deglitch;

  m_qtpCriticalAngle = params.criticalAngle;
}

AprilTagDetector::QuadThresholdParameters
AprilTagDetector::GetQuadThresholdParameters() const {
  auto& qtp = static_cast<apriltag_detector_t*>(m_impl)->qtp;
  return {
      .minClusterPixels = qtp.min_cluster_pixels,
      .maxNumMaxima = qtp.max_nmaxima,
      .criticalAngle = m_qtpCriticalAngle,
      .maxLineFitMSE = qtp.max_line_fit_mse,
      .minWhiteBlackDiff = qtp.min_white_black_diff,
      .deglitch = qtp.deglitch != 0,
  };
}

bool AprilTagDetector::AddFamily(std::string_view fam, int bitsCorrected) {
  auto& data = m_families[fam];
  if (data) {
    return true;  // already detecting
  }
  // create the family
  if (fam == "tag16h5") {
    data = tag16h5_create();
  } else if (fam == "tag36h11") {
    data = tag36h11_create();
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
    DestroyFamily(it->first, it->second);
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
    DestroyFamily(entry.first, entry.second);
  }
}

void AprilTagDetector::DestroyFamily(std::string_view name, void* data) {
  auto fam = static_cast<apriltag_family_t*>(data);
  if (name == "tag16h5") {
    tag16h5_destroy(fam);
  } else if (name == "tag36h11") {
    tag36h11_destroy(fam);
  }
}
