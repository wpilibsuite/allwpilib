/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * Creates a new vision runner. It will take images from the {@code
 * videoSource}, send them to the {@code pipeline}, and call the {@code
 * listener} when the pipeline has finished to alert user code when it is safe
 * to access the pipeline's outputs.
 *
 * @param videoSource The video source to use to supply images for the pipeline
 * @param pipeline    The vision pipeline to run
 * @param listener    A function to call after the pipeline has finished running
 */
template <typename T>
VisionRunner<T>::VisionRunner(cs::VideoSource videoSource, T* pipeline,
                              std::function<void(T&)> listener)
    : VisionRunnerBase(videoSource),
      m_pipeline(pipeline),
      m_listener(listener) {}

template <typename T>
void VisionRunner<T>::DoProcess(cv::Mat& image) {
  m_pipeline->Process(image);
  m_listener(*m_pipeline);
}

}  // namespace frc
