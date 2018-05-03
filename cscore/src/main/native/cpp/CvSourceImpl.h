/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef ALLWPILIB_CVSOURCEIMPL_H_
#define ALLWPILIB_CVSOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "SourceImpl.h"

namespace cs {

class CvSourceImpl : public SourceImpl {
 public:
  CvSourceImpl(wpi::StringRef name, const VideoMode& mode);
  ~CvSourceImpl() override;

  void Start();

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, wpi::StringRef value,
                         CS_Status* status) override;

  // Standard common camera properties
  void SetBrightness(int brightness, CS_Status* status) override;
  int GetBrightness(CS_Status* status) const override;
  void SetWhiteBalanceAuto(CS_Status* status) override;
  void SetWhiteBalanceHoldCurrent(CS_Status* status) override;
  void SetWhiteBalanceManual(int value, CS_Status* status) override;
  void SetExposureAuto(CS_Status* status) override;
  void SetExposureHoldCurrent(CS_Status* status) override;
  void SetExposureManual(int value, CS_Status* status) override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // OpenCV-specific functions
  void PutFrame(cv::Mat& image);
  void NotifyError(wpi::StringRef msg);
  int CreateProperty(wpi::StringRef name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value);
  int CreateProperty(wpi::StringRef name, CS_PropertyKind kind, int minimum,
                     int maximum, int step, int defaultValue, int value,
                     std::function<void(CS_Property property)> onChange);
  void SetEnumPropertyChoices(int property, wpi::ArrayRef<std::string> choices,
                              CS_Status* status);

  // Property data
  class PropertyData : public PropertyImpl {
   public:
    PropertyData() = default;
    explicit PropertyData(wpi::StringRef name_) : PropertyImpl{name_} {}
    PropertyData(wpi::StringRef name_, CS_PropertyKind kind_, int minimum_,
                 int maximum_, int step_, int defaultValue_, int value_)
        : PropertyImpl{name_, kind_, step_, defaultValue_, value_} {
      hasMinimum = true;
      minimum = minimum_;
      hasMaximum = true;
      maximum = maximum_;
    }
    ~PropertyData() override = default;

    std::function<void(CS_Property property)> onChange;
  };

 protected:
  std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      wpi::StringRef name) const override;

  bool CacheProperties(CS_Status* status) const override;

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // ALLWPILIB_CVSOURCEIMPL_H_
