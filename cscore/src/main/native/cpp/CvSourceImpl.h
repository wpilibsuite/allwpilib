/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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

#include "SourceImpl.h"

namespace cs {

class CvSourceImpl : public SourceImpl {
 public:
  CvSourceImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
               Telemetry& telemetry, const VideoMode& mode);
  ~CvSourceImpl() override;

  void Start() override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // OpenCV-specific functions
  void PutFrame(cv::Mat& image);
  void NotifyError(const wpi::Twine& msg);
  int CreateProperty(const wpi::Twine& name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value);
  int CreateProperty(const wpi::Twine& name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value,
                     std::function<void(CS_Property property)> onChange);
  void SetEnumPropertyChoices(int property, wpi::ArrayRef<std::string> choices,
                              CS_Status* status);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_CVSOURCEIMPL_H_
