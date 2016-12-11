/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "DriverStation.h"
#include "RobotBase.h"

namespace frc {

/**
  * Creates a new vision runner. It will take images from the {@code
  * videoSource}, send them to the {@code pipeline}, and call the {@code
  * listener} when the pipeline has finished to alert user code when it is safe
  * to access the pipeline's outputs.
  *
  * @param videoSource the video source to use to supply images for the pipeline
  * @param pipeline    the vision pipeline to run
  * @param listener    a function to call after the pipeline has finished
 * running
  */
template <typename T>
VisionRunner<T>::VisionRunner(cs::VideoSource videoSource, T* pipeline,
                              std::function<void(T&)> listener)
    : m_image(),
      m_cvSink("VisionRunner CvSink"),
      m_pipeline(pipeline),
      m_listener(listener) {
  m_cvSink.SetSource(videoSource);
}

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
template <typename T>
void VisionRunner<T>::RunOnce() {
  if (std::this_thread::get_id() == RobotBase::GetThreadId()) {
    wpi_setErrnoErrorWithContext(
        "VisionRunner::RunForever() cannot be called from the main robot "
        "thread");
    return;
  }
  auto frameTime = m_cvSink.GrabFrame(m_image);
  if (frameTime == 0) {
    auto error = m_cvSink.GetError();
    DriverStation::ReportError(error);
  } else {
    m_pipeline->Process(m_image);
    m_listener(*m_pipeline);
  }
}

/**
  * A convenience method that calls {@link #runOnce()} in an infinite loop. This
  * must be run in a dedicated thread, and cannot be used in the main robot
  * thread because it will freeze the robot program.
  *
  * <p><strong>Do not call this method directly from the main
  * thread.</strong></p>
  */
template <typename T>
void VisionRunner<T>::RunForever() {
  if (std::this_thread::get_id() == RobotBase::GetThreadId()) {
    wpi_setErrnoErrorWithContext(
        "VisionRunner::RunForever() cannot be called from the main robot "
        "thread");
    return;
  }
  while (true) {
    RunOnce();
  }
}
}  // namespace frc
