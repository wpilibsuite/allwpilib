/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "vision/VisionRunner.h"

#include <opencv2/core/mat.hpp>

#include "DriverStation.h"
#include "RobotBase.h"

using namespace frc;

/**
 * Creates a new vision runner. It will take images from the {@code
 * videoSource}, and call the virtual DoProcess() method.
 *
 * @param videoSource the video source to use to supply images for the pipeline
 */
VisionRunnerBase::VisionRunnerBase(cs::VideoSource videoSource)
    : m_image(std::make_unique<cv::Mat>()),
      m_cvSink("VisionRunner CvSink"),
      m_enabled(true) {
  m_cvSink.SetSource(videoSource);
}

// Located here and not in header due to cv::Mat forward declaration.
VisionRunnerBase::~VisionRunnerBase() {}

/**
 * Runs the pipeline one time, giving it the next image from the video source
 * specified in the constructor. This will block until the source either has an
 * image or throws an error. If the source successfully supplied a frame, the
 * pipeline's image input will be set, the pipeline will run, and the listener
 * specified in the constructor will be called to notify it that the pipeline
 * ran. This must be run in a dedicated thread, and cannot be used in the main
 * robot thread because it will freeze the robot program.
 *
 * <p>This method is exposed to allow teams to add additional functionality or
 * have their own ways to run the pipeline. Most teams, however, should just
 * use {@link #runForever} in its own thread using a std::thread.</p>
 */
void VisionRunnerBase::RunOnce() {
  if (std::this_thread::get_id() == RobotBase::GetThreadId()) {
    wpi_setErrnoErrorWithContext(
        "VisionRunner::RunOnce() cannot be called from the main robot thread");
    return;
  }
  auto frameTime = m_cvSink.GrabFrame(*m_image);
  if (frameTime == 0) {
    auto error = m_cvSink.GetError();
    DriverStation::ReportError(error);
  } else {
    DoProcess(*m_image);
  }
}

/**
 * A convenience method that calls {@link #runOnce()} in an infinite loop. This
 * must be run in a dedicated thread, and cannot be used in the main robot
 * thread because it will freeze the robot program.
 *
 * <strong>Do not call this method directly from the main thread.</strong>
 */
void VisionRunnerBase::RunForever() {
  if (std::this_thread::get_id() == RobotBase::GetThreadId()) {
    wpi_setErrnoErrorWithContext(
        "VisionRunner::RunForever() cannot be called from the main robot "
        "thread");
    return;
  }
  while (m_enabled) {
    RunOnce();
  }
}

/**
 * Stop a RunForever() loop.
 */
void VisionRunnerBase::Stop() { m_enabled = false; }
