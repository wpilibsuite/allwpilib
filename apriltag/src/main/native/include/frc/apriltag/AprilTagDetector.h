// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string_view>
#include <utility>

#include <units/angle.h>
#include <wpi/StringMap.h>
#include <wpi/SymbolExports.h>

#include "frc/apriltag/AprilTagDetection.h"

namespace frc {

/**
 * An AprilTag detector engine. This is expensive to set up and tear down, so
 * most use cases should only create one of these, add a family to it, set up
 * any other configuration, and repeatedly call Detect().
 */
class WPILIB_DLLEXPORT AprilTagDetector {
 public:
  /** Detector configuration. */
  struct Config {
    bool operator==(const Config&) const = default;

    /**
     * How many threads should be used for computation. Default is
     * single-threaded operation (1 thread).
     */
    int numThreads = 1;

    /**
     * Quad decimation. Detection of quads can be done on a lower-resolution
     * image, improving speed at a cost of pose accuracy and a slight decrease
     * in detection rate. Decoding the binary payload is still done at full
     * resolution. Default is 2.0.
     */
    float quadDecimate = 2.0f;

    /**
     * What Gaussian blur should be applied to the segmented image (used for
     * quad detection). Very noisy images benefit from non-zero values (e.g.
     * 0.8). Default is 0.0.
     */
    float quadSigma = 0.0f;

    /**
     * When true, the edges of the each quad are adjusted to "snap to" strong
     * gradients nearby. This is useful when decimation is employed, as it can
     * increase the quality of the initial quad estimate substantially.
     * Generally recommended to be on (true). Default is true.
     *
     * Very computationally inexpensive. Option is ignored if
     * quad_decimate = 1.
     */
    bool refineEdges = true;

    /**
     * How much sharpening should be done to decoded images. This can help
     * decode small tags but may or may not help in odd lighting conditions or
     * low light conditions. Default is 0.25.
     */
    double decodeSharpening = 0.25;

    /**
     * Debug mode. When true, the decoder writes a variety of debugging images
     * to the current working directory at various stages through the detection
     * process. This is slow and should *not* be used on space-limited systems
     * such as the RoboRIO. Default is disabled (false).
     */
    bool debug = false;
  };

  /** Quad threshold parameters. */
  struct QuadThresholdParameters {
    bool operator==(const QuadThresholdParameters&) const = default;

    /**
     * Threshold used to reject quads containing too few pixels. Default is 300
     * pixels.
     */
    int minClusterPixels = 300;

    /**
     * How many corner candidates to consider when segmenting a group of pixels
     * into a quad. Default is 10.
     */
    int maxNumMaxima = 10;

    /**
     * Critical angle. The detector will reject quads where pairs of edges have
     * angles that are close to straight or close to 180 degrees. Zero means
     * that no quads are rejected. Default is 45 degrees.
     */
    units::radian_t criticalAngle = 45_deg;

    /**
     * When fitting lines to the contours, the maximum mean squared error
     * allowed. This is useful in rejecting contours that are far from being
     * quad shaped; rejecting these quads "early" saves expensive decoding
     * processing. Default is 10.0.
     */
    float maxLineFitMSE = 10.0f;

    /**
     * Minimum brightness offset. When we build our model of black & white
     * pixels, we add an extra check that the white model must be (overall)
     * brighter than the black model. How much brighter? (in pixel values,
     * [0,255]). Default is 5.
     */
    int minWhiteBlackDiff = 5;

    /**
     * Whether the thresholded image be should be deglitched. Only useful for
     * very noisy images. Default is disabled (false).
     */
    bool deglitch = false;
  };

  /**
   * Array of detection results. Each array element is a pointer to an
   * AprilTagDetection.
   */
  class WPILIB_DLLEXPORT Results
      : public std::span<AprilTagDetection const* const> {
    struct private_init {};
    friend class AprilTagDetector;

   public:
    Results() = default;
    Results(void* impl, const private_init&);
    ~Results() { Destroy(); }
    Results(const Results&) = delete;
    Results& operator=(const Results&) = delete;
    Results(Results&& rhs) : span{std::move(rhs)}, m_impl{rhs.m_impl} {
      rhs.m_impl = nullptr;
    }
    Results& operator=(Results&& rhs);

   private:
    void Destroy();
    void* m_impl = nullptr;
  };

  AprilTagDetector();
  ~AprilTagDetector() { Destroy(); }
  AprilTagDetector(const AprilTagDetector&) = delete;
  AprilTagDetector& operator=(const AprilTagDetector&) = delete;
  AprilTagDetector(AprilTagDetector&& rhs)
      : m_impl{rhs.m_impl},
        m_families{std::move(rhs.m_families)},
        m_qtpCriticalAngle{rhs.m_qtpCriticalAngle} {
    rhs.m_impl = nullptr;
  }
  AprilTagDetector& operator=(AprilTagDetector&& rhs);

  /**
   * @{
   * @name Configuration functions
   */

  /**
   * Sets detector configuration.
   *
   * @param config Configuration
   */
  void SetConfig(const Config& config);

  /**
   * Gets detector configuration.
   *
   * @return Configuration
   */
  Config GetConfig() const;

  /**
   * Sets quad threshold parameters.
   *
   * @param params Parameters
   */
  void SetQuadThresholdParameters(const QuadThresholdParameters& params);

  /**
   * Gets quad threshold parameters.
   *
   * @return Parameters
   */
  QuadThresholdParameters GetQuadThresholdParameters() const;

  /** @} */

  /**
   * @{
   * @name Tag family functions
   */

  /**
   * Adds a family of tags to be detected.
   *
   * @param fam Family name, e.g. "tag16h5"
   * @param bitsCorrected Maximum number of bits to correct
   * @return False if family can't be found
   */
  bool AddFamily(std::string_view fam, int bitsCorrected = 2);

  /**
   * Removes a family of tags from the detector.
   *
   * @param fam Family name, e.g. "tag16h5"
   */
  void RemoveFamily(std::string_view fam);

  /**
   * Unregister all families.
   */
  void ClearFamilies();

  /** @} */

  /**
   * Detect tags from an 8-bit image.
   * The image must be grayscale.
   *
   * @param width width of the image
   * @param height height of the image
   * @param stride number of bytes between image rows (often the same as width)
   * @param buf image buffer
   * @return Results (array of AprilTagDetection pointers)
   */
  Results Detect(int width, int height, int stride, uint8_t* buf);

  /**
   * Detect tags from an 8-bit image.
   * The image must be grayscale.
   *
   * @param width width of the image
   * @param height height of the image
   * @param buf image buffer
   * @return Results (array of AprilTagDetection pointers)
   */
  Results Detect(int width, int height, uint8_t* buf) {
    return Detect(width, height, width, buf);
  }

 private:
  void Destroy();
  void DestroyFamilies();
  void DestroyFamily(std::string_view name, void* data);

  void* m_impl;
  wpi::StringMap<void*> m_families;
  units::radian_t m_qtpCriticalAngle = 10_deg;
};

}  // namespace frc
