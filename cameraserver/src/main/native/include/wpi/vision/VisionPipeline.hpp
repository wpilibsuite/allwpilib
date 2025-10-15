// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace cv {
class Mat;
}  // namespace cv

namespace frc {

/**
 * A vision pipeline is responsible for running a group of OpenCV algorithms to
 * extract data from an image.
 *
 * @see VisionRunner
 */
class VisionPipeline {
 public:
  virtual ~VisionPipeline() = default;

  /**
   * Processes the image input and sets the result objects. Implementations
   * should make these objects accessible.
   */
  virtual void Process(cv::Mat& mat) = 0;
};
}  // namespace frc
