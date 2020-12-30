// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "vision/VisionRunner.h"

#include <thread>

#include <opencv2/core/mat.hpp>

#include "cameraserver/CameraServerShared.h"

using namespace frc;

VisionRunnerBase::VisionRunnerBase(cs::VideoSource videoSource)
    : m_image(std::make_unique<cv::Mat>()),
      m_cvSink("VisionRunner CvSink"),
      m_enabled(true) {
  m_cvSink.SetSource(videoSource);
}

// Located here and not in header due to cv::Mat forward declaration.
VisionRunnerBase::~VisionRunnerBase() = default;

void VisionRunnerBase::RunOnce() {
  auto csShared = frc::GetCameraServerShared();
  auto res = csShared->GetRobotMainThreadId();
  if (res.second && (std::this_thread::get_id() == res.first)) {
    csShared->SetVisionRunnerError(
        "VisionRunner::RunOnce() cannot be called from the main robot thread");
    return;
  }
  auto frameTime = m_cvSink.GrabFrame(*m_image);
  if (frameTime == 0) {
    auto error = m_cvSink.GetError();
    csShared->ReportDriverStationError(error);
  } else {
    DoProcess(*m_image);
  }
}

void VisionRunnerBase::RunForever() {
  auto csShared = frc::GetCameraServerShared();
  auto res = csShared->GetRobotMainThreadId();
  if (res.second && (std::this_thread::get_id() == res.first)) {
    csShared->SetVisionRunnerError(
        "VisionRunner::RunForever() cannot be called from the main robot "
        "thread");
    return;
  }
  while (m_enabled) {
    RunOnce();
  }
}

void VisionRunnerBase::Stop() {
  m_enabled = false;
}
