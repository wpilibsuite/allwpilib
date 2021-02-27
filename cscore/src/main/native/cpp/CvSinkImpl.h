// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CVSINKIMPL_H_
#define CSCORE_CVSINKIMPL_H_

#include <stdint.h>

#include <atomic>
#include <functional>
#include <thread>

#include <opencv2/core/core.hpp>
#include <wpi/Twine.h>
#include <wpi/condition_variable.h>

#include "Frame.h"
#include "SinkImpl.h"

namespace cs {

class SourceImpl;

class CvSinkImpl : public SinkImpl {
 public:
  CvSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
             Telemetry& telemetry);
  CvSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
             Telemetry& telemetry,
             std::function<void(uint64_t time)> processFrame);
  ~CvSinkImpl() override;

  void Stop();

  uint64_t GrabFrame(cv::Mat& image);
  uint64_t GrabFrame(cv::Mat& image, double timeout);

 private:
  void ThreadMain();

  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_thread;
  std::function<void(uint64_t time)> m_processFrame;
};

}  // namespace cs

#endif  // CSCORE_CVSINKIMPL_H_
