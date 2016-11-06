/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_CVSOURCEIMPL_H_
#define CS_CVSOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <vector>

#include "SourceImpl.h"

namespace cs {

class CvSourceImpl : public SourceImpl {
 public:
  CvSourceImpl(llvm::StringRef name, const VideoMode& mode);
  ~CvSourceImpl() override;

  bool IsConnected() const override;

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, llvm::StringRef value,
                         CS_Status* status) override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // OpenCV-specific functions
  void PutFrame(cv::Mat& image);
  void NotifyError(llvm::StringRef msg);
  void SetConnected(bool connected);
  CS_Property CreateProperty(llvm::StringRef name, CS_PropertyType type,
                             int minimum, int maximum, int step,
                             int defaultValue, int value);
  CS_Property CreateProperty(
      llvm::StringRef name, CS_PropertyType type, int minimum, int maximum,
      int step, int defaultValue, int value,
      std::function<void(CS_Property property)> onChange);
  void SetEnumPropertyChoices(CS_Property property,
                              llvm::ArrayRef<std::string> choices,
                              CS_Status* status);
  void RemoveProperty(CS_Property property);
  void RemoveProperty(llvm::StringRef name);

  // Property data
  class PropertyData : public PropertyBase {
   public:
    PropertyData() = default;
    PropertyData(llvm::StringRef name_, CS_PropertyType type_, int minimum_,
                 int maximum_, int step_, int defaultValue_, int value_)
        : PropertyBase{name_, type_,         minimum_, maximum_,
                       step_, defaultValue_, value_} {}
    ~PropertyData() override = default;

    std::function<void(CS_Property property)> onChange;
  };

 protected:
  bool CacheProperties(CS_Status* status) const override;

 private:
  std::atomic_bool m_connected{true};
  std::vector<unsigned char> m_jpegBuf;
  std::vector<int> m_compressionParams;
  int m_qualityProperty;
};

}  // namespace cs

#endif  // CS_CVSOURCEIMPL_H_
