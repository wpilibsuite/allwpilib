/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
