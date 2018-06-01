/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <functional>
#include <memory>

#include "cscore.h"
#include "vision/VisionPipeline.h"

namespace frc {

/**
 * Non-template base class for VisionRunner.
 */
class VisionRunnerBase {
 public:
  /**
   * Creates a new vision runner. It will take images from the {@code
   * videoSource}, and call the virtual DoProcess() method.
   *
   * @param videoSource the video source to use to supply images for the
   *                    pipeline
   */
  explicit VisionRunnerBase(cs::VideoSource videoSource);

  ~VisionRunnerBase();

  VisionRunnerBase(const VisionRunnerBase&) = delete;
  VisionRunnerBase& operator=(const VisionRunnerBase&) = delete;

  /**
   * Runs the pipeline one time, giving it the next image from the video source
   * specified in the constructor. This will block until the source either has
   * an image or throws an error. If the source successfully supplied a frame,
   * the pipeline's image input will be set, the pipeline will run, and the
   * listener specified in the constructor will be called to notify it that the
   * pipeline ran. This must be run in a dedicated thread, and cannot be used in
   * the main robot thread because it will freeze the robot program.
   *
   * <p>This method is exposed to allow teams to add additional functionality or
   * have their own ways to run the pipeline. Most teams, however, should just
   * use {@link #runForever} in its own thread using a std::thread.</p>
   */
  void RunOnce();

  /**
   * A convenience method that calls {@link #runOnce()} in an infinite loop.
   * This must be run in a dedicated thread, and cannot be used in the main
   * robot thread because it will freeze the robot program.
   *
   * <strong>Do not call this method directly from the main thread.</strong>
   */
  void RunForever();

  /**
   * Stop a RunForever() loop.
   */
  void Stop();

 protected:
  virtual void DoProcess(cv::Mat& image) = 0;

 private:
  std::unique_ptr<cv::Mat> m_image;
  cs::CvSink m_cvSink;
  std::atomic_bool m_enabled;
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
