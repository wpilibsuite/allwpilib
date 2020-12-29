// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CVSOURCEIMPL_H_
#define CSCORE_CVSOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <wpi/ArrayRef.h>
#include <wpi/Twine.h>

#include "ConfigurableSourceImpl.h"
#include "SourceImpl.h"

namespace cs {

class CvSourceImpl : public ConfigurableSourceImpl {
 public:
  CvSourceImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
               Telemetry& telemetry, const VideoMode& mode);
  ~CvSourceImpl() override;

  // OpenCV-specific functions
  void PutFrame(cv::Mat& image);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_CVSOURCEIMPL_H_
