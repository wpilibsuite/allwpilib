/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include "ErrorBase.h"
#include "cscore.h"
#include "opencv2/core/mat.hpp"
#include "vision/VisionPipeline.h"

namespace frc {

/**
 * A vision runner is a convenient wrapper object to make it easy to run vision
 * pipelines from robot code. The easiest  way to use this is to run it in a
 * std::thread and use the listener to take snapshots of the pipeline's outputs.
 *
 * @see VisionPipeline
 */
template <typename T>
class VisionRunner : public ErrorBase {
 public:
  VisionRunner(cs::VideoSource videoSource, T* pipeline,
               std::function<void(T&)> listener);
  virtual ~VisionRunner() = default;

  VisionRunner(const VisionRunner&) = delete;
  VisionRunner& operator=(const VisionRunner&) = delete;

  void RunOnce();

  void RunForever();

 private:
  cv::Mat m_image;
  cs::CvSink m_cvSink;
  T* m_pipeline;
  std::function<void(T&)> m_listener;
};
}  // namespace frc

#include "VisionRunner.inc"
