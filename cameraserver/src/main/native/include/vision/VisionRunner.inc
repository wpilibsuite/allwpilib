// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "vision/VisionRunner.h"

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
