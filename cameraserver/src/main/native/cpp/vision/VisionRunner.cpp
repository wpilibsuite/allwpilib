/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
VisionRunnerBase::~VisionRunnerBase() {}

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

void VisionRunnerBase::Stop() { m_enabled = false; }
