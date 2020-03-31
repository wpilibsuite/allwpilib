/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_IMAGESOURCEIMPL_H_
#define CSCORE_IMAGESOURCEIMPL_H_

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <wpi/ArrayRef.h>
#include <wpi/Twine.h>

#include "SourceImpl.h"
#include "cscore_raw.h"

namespace cs {

class ImageSourceImpl : public SourceImpl {
 public:
  ImageSourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                  const VideoMode& mode);
  ~ImageSourceImpl() override;

  void Start() override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // Image-specific functions
  void NotifyError(const wpi::Twine& msg);
  int CreateProperty(const wpi::Twine& name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value);
  void SetEnumPropertyChoices(int property, wpi::ArrayRef<std::string> choices,
                              CS_Status* status);

  void PutFrame(cv::Mat& image);
  void PutFrame(const CS_RawFrame& image);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_IMAGESOURCEIMPL_H_
