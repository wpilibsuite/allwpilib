/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>

#include "ErrorBase.h"
#include "cscore.h"
#include "vision/VisionPipeline.h"

namespace frc {

/**
 * Non-template base class for VisionRunner.
 */
class VisionRunnerBase : public ErrorBase {
 public:
  explicit VisionRunnerBase(cs::VideoSource videoSource);
  ~VisionRunnerBase() override;

  VisionRunnerBase(const VisionRunnerBase&) = delete;
  VisionRunnerBase& operator=(const VisionRunnerBase&) = delete;

  void RunOnce();

  void RunForever();

 protected:
  virtual void DoProcess(cv::Mat& image) = 0;

 private:
  std::unique_ptr<cv::Mat> m_image;
  cs::CvSink m_cvSink;
};

/**
 * A vision runner is a convenient wrapper object to make it easy to run vision
 * pipelines from robot code. The easiest way to use this is to run it in a
 * std::thread and use the listener to take snapshots of the pipeline's outputs.
 *
 * @see VisionPipeline
 */
template <typename T>
class VisionRunner : public VisionRunnerBase {
 public:
  VisionRunner(cs::VideoSource videoSource, T* pipeline,
               std::function<void(T&)> listener);
  virtual ~VisionRunner() = default;

 protected:
  void DoProcess(cv::Mat& image) override;

 private:
  T* m_pipeline;
  std::function<void(T&)> m_listener;
};
}  // namespace frc

#include "VisionRunner.inc"
