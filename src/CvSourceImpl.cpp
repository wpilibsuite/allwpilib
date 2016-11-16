/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CvSourceImpl.h"

#include "llvm/STLExtras.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "support/timestamp.h"

#include "cscore_cpp.h"
#include "c_util.h"
#include "Handle.h"
#include "Log.h"

using namespace cs;

CvSourceImpl::CvSourceImpl(llvm::StringRef name, const VideoMode& mode)
    : SourceImpl{name} {
  m_mode = mode;
  m_videoModes.push_back(m_mode);

  // Create jpeg quality property
  m_compressionParams.push_back(CV_IMWRITE_JPEG_QUALITY);
  m_compressionParams.push_back(80);

  m_qualityProperty =
      CreateProperty("jpeg_quality", CS_PROP_INTEGER, 0, 100, 1, 80, 80);
}

CvSourceImpl::~CvSourceImpl() {}

bool CvSourceImpl::IsConnected() const { return m_connected; }

bool CvSourceImpl::CacheProperties(CS_Status* status) const {
  // Doesn't need to do anything.
  m_properties_cached = true;
  return true;
}

void CvSourceImpl::SetProperty(int property, int value, CS_Status* status) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = static_cast<PropertyData*>(GetProperty(property));
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }
  if ((prop->propType & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }
  prop->value = value;
}

void CvSourceImpl::SetStringProperty(int property, llvm::StringRef value,
                                     CS_Status* status) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = static_cast<PropertyData*>(GetProperty(property));
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }
  if (prop->propType != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }
  prop->valueStr = value;
}

bool CvSourceImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  // can't set video mode on OpenCV source
  return false;
}

void CvSourceImpl::NumSinksChanged() {
  // ignore
}

void CvSourceImpl::NumSinksEnabledChanged() {
  // ignore
}

void CvSourceImpl::PutFrame(cv::Mat& image) {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (auto prop = GetProperty(m_qualityProperty)) {
    if (prop->value >= 0 && prop->value <= 100)
      m_compressionParams[1] = prop->value;
  }
  cv::imencode(".jpg", image, m_jpegBuf, m_compressionParams);
  SourceImpl::PutFrame(
      VideoMode::kMJPEG, image.cols, image.rows,
      llvm::StringRef(reinterpret_cast<const char*>(m_jpegBuf.data()),
                      m_jpegBuf.size()),
      wpi::Now());
}

void CvSourceImpl::NotifyError(llvm::StringRef msg) {
  PutError(msg, wpi::Now());
}

void CvSourceImpl::SetConnected(bool connected) { m_connected = connected; }

CS_Property CvSourceImpl::CreateProperty(llvm::StringRef name,
                                         CS_PropertyType type, int minimum,
                                         int maximum, int step,
                                         int defaultValue, int value) {
  std::unique_lock<std::mutex> lock(m_mutex);
  int& ndx = m_properties[name];
  if (ndx == 0) {
    // create a new index
    ndx = m_propertyData.size() + 1;
    m_propertyData.emplace_back(llvm::make_unique<PropertyData>(
        name, type, minimum, maximum, step, defaultValue, value));
  } else {
    // update all but value
    auto prop = GetProperty(ndx);
    prop->propType = type;
    prop->minimum = minimum;
    prop->maximum = maximum;
    prop->step = step;
    prop->defaultValue = defaultValue;
  }
  return ndx;
}

CS_Property CvSourceImpl::CreateProperty(
    llvm::StringRef name, CS_PropertyType type, int minimum, int maximum,
    int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange) {
  // TODO
  return 0;
}

void CvSourceImpl::SetEnumPropertyChoices(CS_Property property,
                                          llvm::ArrayRef<std::string> choices,
                                          CS_Status* status) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }
  if (prop->propType != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }
  prop->enumChoices = choices;
}

namespace cs {

CS_Source CreateCvSource(llvm::StringRef name, const VideoMode& mode,
                         CS_Status* status) {
  auto source = std::make_shared<CvSourceImpl>(name, mode);
  return Sources::GetInstance().Allocate(CS_SOURCE_CV, source);
}

void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).PutFrame(image);
}

void NotifySourceError(CS_Source source, llvm::StringRef msg,
                       CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, llvm::StringRef description,
                          CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetDescription(description);
}

CS_Property CreateSourceProperty(CS_Source source, llvm::StringRef name,
                                 CS_PropertyType type, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  return static_cast<CvSourceImpl&>(*data->source)
      .CreateProperty(name, type, minimum, maximum, step, defaultValue, value);
}

CS_Property CreateSourcePropertyCallback(
    CS_Source source, llvm::StringRef name, CS_PropertyType type, int minimum,
    int maximum, int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  return static_cast<CvSourceImpl&>(*data->source)
      .CreateProperty(name, type, minimum, maximum, step, defaultValue, value,
                      onChange);
}

void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  llvm::ArrayRef<std::string> choices,
                                  CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->type != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source)
      .SetEnumPropertyChoices(property, choices, status);
}

}  // namespace cs

extern "C" {

CS_Source CS_CreateCvSource(const char* name, const CS_VideoMode* mode,
                            CS_Status* status) {
  return cs::CreateCvSource(name, static_cast<const cs::VideoMode&>(*mode),
                            status);
}

void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::PutSourceFrame(source, mat, status);
}

void CS_NotifySourceError(CS_Source source, const char* msg,
                          CS_Status* status) {
  return cs::NotifySourceError(source, msg, status);
}

void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status) {
  return cs::SetSourceConnected(source, connected, status);
}

void CS_SetSourceDescription(CS_Source source, const char* description,
                             CS_Status* status) {
  return cs::SetSourceDescription(source, description, status);
}

CS_Property CS_CreateSourceProperty(CS_Source source, const char* name,
                                    enum CS_PropertyType type, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value, CS_Status* status) {
  return cs::CreateSourceProperty(source, name, type, minimum, maximum, step,
                                  defaultValue, value, status);
}

CS_Property CS_CreateSourcePropertyCallback(
    CS_Source source, const char* name, enum CS_PropertyType type, int minimum,
    int maximum, int step, int defaultValue, int value, void* data,
    void (*onChange)(void* data, CS_Property property), CS_Status* status) {
  return cs::CreateSourcePropertyCallback(
      source, name, type, minimum, maximum, step, defaultValue, value,
      [=](CS_Property property) { onChange(data, property); }, status);
}

void CS_SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                     const char** choices, int count,
                                     CS_Status* status) {
  llvm::SmallVector<std::string, 8> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) vec.push_back(choices[i]);
  return cs::SetSourceEnumPropertyChoices(source, property, vec, status);
}

}  // extern "C"
