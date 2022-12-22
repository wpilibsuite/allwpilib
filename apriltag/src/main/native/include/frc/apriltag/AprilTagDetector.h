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
  /**
   * Array of detection results. Each array element is a pointer to an
   * AprilTagDetection.
   */
  class Results : public std::span<AprilTagDetection const* const> {
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
      : m_impl{rhs.m_impl}, m_families{std::move(rhs.m_families)} {
    rhs.m_impl = nullptr;
  }
  AprilTagDetector& operator=(AprilTagDetector&& rhs);

  /**
   * @{
   * @name User-configurable parameters
   */

  /**
   * Sets how many threads should be used for computation. Default is
   * single-threaded operation (1 thread).
   *
   * @param val number of threads
   */
  void SetNumThreads(int val);

  /**
   * Gets how many threads are being used for computation.
   *
   * @return number of threads
   */
  int GetNumThreads() const;

  /**
   * Sets the quad decimation. Detection of quads can be done on a
   * lower-resolution image, improving speed at a cost of pose accuracy and a
   * slight decrease in detection rate. Decoding the binary payload is still
   * done at full resolution. Default is 2.0.
   *
   * @param val decimation amount
   */
  void SetQuadDecimate(float val);

  /**
   * Gets the quad decimation setting.
   *
   * @return decimation amount
   */
  float GetQuadDecimate() const;

  /**
   * Sets what Gaussian blur should be applied to the segmented image
   * (used for quad detection?). Very noisy images benefit from non-zero values
   * (e.g. 0.8). Default is 0.0.
   *
   * @param val standard deviation in pixels
   */
  void SetQuadSigma(float val);

  /**
   * Gets the Guassian blur setting.
   *
   * @return standard deviation in pixels
   */
  float GetQuadSigma() const;

  /**
   * Sets the refine edges option. When true, the edges of the each quad are
   * adjusted to "snap to" strong gradients nearby. This is useful when
   * decimation is employed, as it can increase the quality of the initial quad
   * estimate substantially. Generally recommended to be on (true). Default is
   * true.
   *
   * Very computationally inexpensive. Option is ignored if
   * quad_decimate = 1.
   *
   * @param val true to enable refine edges
   */
  void SetRefineEdges(bool val);

  /**
   * Gets the refine edges option setting.
   *
   * @return true if refine edges is enabled
   */
  bool GetRefineEdges() const;

  /**
   * Sets how much sharpening should be done to decoded images. This
   * can help decode small tags but may or may not help in odd
   * lighting conditions or low light conditions. Default is 0.25.
   *
   * @param val sharpening amount
   */
  void SetDecodeSharpening(double val);

  /**
   * Gets the decode sharpening setting.
   *
   * @return sharpening amount
   */
  double GetDecodeSharpening() const;

  /**
   * Sets debug mode. When true, the decoder writes a variety of debugging
   * images to the current working directory at various stages through the
   * detection process. This is slow and should *not* be used on space-limited
   * systems such as the RoboRIO. Default is disabled (false).
   *
   * @param val true to enable debug mode
   */
  void SetDebug(bool val);

  /**
   * Gets debug mode setting.
   *
   * @return true if debug mode is enabled
   */
  bool GetDebug() const;

  /** @} */

  /**
   * @{
   * @name Quad threshold parameters
   */

  /**
   * Sets threshold used to reject quads containing too few pixels. Default is 5
   * pixels.
   *
   * @param val minimum number of pixels
   */
  void SetQuadMinClusterPixels(int val);

  /**
   * Gets minimum number of pixel threshold setting.
   *
   * @return number of pixels
   */
  int GetQuadMinClusterPixels() const;

  /**
   * Sets how many corner candidates to consider when segmenting a group
   * of pixels into a quad. Default is 10.
   *
   * @param val number of candidates
   */
  void SetQuadMaxNumMaxima(int val);

  /**
   * Gets corner candidates setting.
   *
   * @return number of candidates
   */
  int GetQuadMaxNumMaxima() const;

  /**
   * Sets critical angle. The detector will reject quads where pairs of edges
   * have angles that are close to straight or close to 180 degrees. Zero means
   * that no quads are rejected. Default is 10 degrees.
   *
   * @param val critical angle
   */
  void SetQuadCriticalAngle(units::radian_t val);

  /**
   * Gets critical angle setting.
   *
   * @return critical angle
   */
  units::radian_t GetQuadCriticalAngle() const;

  /**
   * When fitting lines to the contours, sets the maximum mean squared error
   * allowed. This is useful in rejecting contours that are far from being quad
   * shaped; rejecting these quads "early" saves expensive decoding processing.
   * Default is 10.0.
   *
   * @param val allowable maximum mean squared error
   */
  void SetQuadMaxLineFitMSE(float val);

  /**
   * Gets the maximum mean squared error setting.
   *
   * @return allowable maximum mean squared error
   */
  float GetQuadMaxLineFitMSE() const;

  /**
   * Sets minimum brightness offset. When we build our model of black & white
   * pixels, we add an extra check that the white model must be (overall)
   * brighter than the black model. How much brighter? (in pixel values,
   * [0,255]). Default is 5.
   *
   * @param val minimum offset in pixel values [0,255]
   */
  void SetQuadMinWhiteBlackDiff(int val);

  /**
   * Gets minimum brightness offset setting.
   *
   * @return minimum offset in pixel values [0, 255]
   */
  int GetQuadMinWhiteBlackDiff() const;

  /**
   * Sets if the thresholded image be should be deglitched. Only useful for
   * very noisy images. Default is disabled (false).
   *
   * @param val true to enable
   */
  void SetQuadDeglitch(bool val);

  /**
   * Gets the deglitch setting.
   *
   * @return true if enabled
   */
  bool GetQuadDeglitch() const;

  /** @} */

  /**
   * @{
   * @name Tag family functions
   */

  /**
   * Adds a family of tags to be detected.
   *
   * @param fam Family name, e.g. "tag16h5"
   * @param bitsCorrected
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
};

}  // namespace frc
