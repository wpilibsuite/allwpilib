/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CvSourceImpl.h"

#include "llvm/STLExtras.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "support/timestamp.h"

#include "cscore_cpp.h"
#include "c_util.h"
#include "Handle.h"
#include "Log.h"
#include "Notifier.h"

using namespace cs;

CvSourceImpl::CvSourceImpl(llvm::StringRef name, const VideoMode& mode)
    : SourceImpl{name} {
  m_mode = mode;
  m_videoModes.push_back(m_mode);
}

CvSourceImpl::~CvSourceImpl() {}

void CvSourceImpl::Start() {}

std::unique_ptr<PropertyImpl> CvSourceImpl::CreateEmptyProperty(
    llvm::StringRef name) const {
  return llvm::make_unique<PropertyData>(name);
}

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

  // Guess it's integer if we've set before get
  if (prop->propKind == CS_PROP_NONE) prop->propKind = CS_PROP_INTEGER;

  if ((prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }

  UpdatePropertyValue(property, false, value, llvm::StringRef{});
}

void CvSourceImpl::SetStringProperty(int property, llvm::StringRef value,
                                     CS_Status* status) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = static_cast<PropertyData*>(GetProperty(property));
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }

  // Guess it's string if we've set before get
  if (prop->propKind == CS_PROP_NONE) prop->propKind = CS_PROP_STRING;

  if (prop->propKind != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }

  UpdatePropertyValue(property, true, 0, value);
}

// These are only valid for cameras (should never get called)

void CvSourceImpl::SetBrightness(int brightness, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

int CvSourceImpl::GetBrightness(CS_Status* status) const {
  *status = CS_INVALID_HANDLE;
  return 0;
}

void CvSourceImpl::SetWhiteBalanceAuto(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void CvSourceImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void CvSourceImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void CvSourceImpl::SetExposureAuto(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void CvSourceImpl::SetExposureHoldCurrent(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

void CvSourceImpl::SetExposureManual(int value, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
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
  // We only support 8-bit images; convert if necessary.
  cv::Mat finalImage;
  if (image.depth() == CV_8U)
    finalImage = image;
  else
    image.convertTo(finalImage, CV_8U);

  std::unique_ptr<Image> dest;
  switch (image.channels()) {
    case 1:
      dest =
          AllocImage(VideoMode::kGray, image.cols, image.rows, image.total());
      finalImage.copyTo(dest->AsMat());
      break;
    case 3:
      dest = AllocImage(VideoMode::kBGR, image.cols, image.rows,
                        image.total() * 3);
      finalImage.copyTo(dest->AsMat());
      break;
    case 4:
      dest = AllocImage(VideoMode::kBGR, image.cols, image.rows,
                        image.total() * 3);
      cv::cvtColor(finalImage, dest->AsMat(), cv::COLOR_BGRA2BGR);
      break;
    default:
      SERROR("PutFrame: " << image.channels()
                          << "-channel images not supported");
      return;
  }
  SourceImpl::PutFrame(std::move(dest), wpi::Now());
}

void CvSourceImpl::NotifyError(llvm::StringRef msg) {
  PutError(msg, wpi::Now());
}

int CvSourceImpl::CreateProperty(llvm::StringRef name, CS_PropertyKind kind,
                                 int minimum, int maximum, int step,
                                 int defaultValue, int value) {
  std::lock_guard<std::mutex> lock(m_mutex);
  int& ndx = m_properties[name];
  if (ndx == 0) {
    // create a new index
    ndx = m_propertyData.size() + 1;
    m_propertyData.emplace_back(llvm::make_unique<PropertyData>(
        name, kind, minimum, maximum, step, defaultValue, value));
  } else {
    // update all but value
    auto prop = GetProperty(ndx);
    prop->propKind = kind;
    prop->minimum = minimum;
    prop->maximum = maximum;
    prop->step = step;
    prop->defaultValue = defaultValue;
    value = prop->value;
  }
  Notifier::GetInstance().NotifySourceProperty(
      *this, CS_SOURCE_PROPERTY_CREATED, name, ndx, kind, value,
      llvm::StringRef{});
  return ndx;
}

int CvSourceImpl::CreateProperty(
    llvm::StringRef name, CS_PropertyKind kind, int minimum, int maximum,
    int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange) {
  // TODO
  return 0;
}

void CvSourceImpl::SetEnumPropertyChoices(int property,
                                          llvm::ArrayRef<std::string> choices,
                                          CS_Status* status) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }
  if (prop->propKind != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }
  prop->enumChoices = choices;
  Notifier::GetInstance().NotifySourceProperty(
      *this, CS_SOURCE_PROPERTY_CHOICES_UPDATED, prop->name, property,
      CS_PROP_ENUM, prop->value, llvm::StringRef{});
}

namespace cs {

CS_Source CreateCvSource(llvm::StringRef name, const VideoMode& mode,
                         CS_Status* status) {
  auto source = std::make_shared<CvSourceImpl>(name, mode);
  auto handle = Sources::GetInstance().Allocate(CS_SOURCE_CV, source);
  auto& notifier = Notifier::GetInstance();
  notifier.NotifySource(name, handle, CS_SOURCE_CREATED);
  // Generate initial events here so they come after the source created event
  source->Start();  // causes a property event
  notifier.NotifySource(name, handle, CS_SOURCE_CONNECTED);
  notifier.NotifySource(name, handle, CS_SOURCE_VIDEOMODES_UPDATED);
  notifier.NotifySourceVideoMode(*source, mode);
  return handle;
}

void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).PutFrame(image);
}

void NotifySourceError(CS_Source source, llvm::StringRef msg,
                       CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, llvm::StringRef description,
                          CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetDescription(description);
}

CS_Property CreateSourceProperty(CS_Source source, llvm::StringRef name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<CvSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value);
  return Handle{source, property, Handle::kProperty};
}

CS_Property CreateSourcePropertyCallback(
    CS_Source source, llvm::StringRef name, CS_PropertyKind kind, int minimum,
    int maximum, int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange, CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<CvSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value, onChange);
  return Handle{source, property, Handle::kProperty};
}

void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  llvm::ArrayRef<std::string> choices,
                                  CS_Status* status) {
  auto data = Sources::GetInstance().Get(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }

  // Get property index; also validate the source owns this property
  Handle handle{property};
  int i = handle.GetParentIndex();
  if (i < 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  auto data2 = Sources::GetInstance().Get(Handle{i, Handle::kSource});
  if (!data2 || data->source.get() != data2->source.get()) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  int propertyIndex = handle.GetIndex();
  static_cast<CvSourceImpl&>(*data->source)
      .SetEnumPropertyChoices(propertyIndex, choices, status);
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

void CS_PutSourceFrameCpp(CS_Source source, cv::Mat* image, 
                          CS_Status* status) {
  return cs::PutSourceFrame(source, *image, status);
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
                                    enum CS_PropertyKind kind, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value, CS_Status* status) {
  return cs::CreateSourceProperty(source, name, kind, minimum, maximum, step,
                                  defaultValue, value, status);
}

CS_Property CS_CreateSourcePropertyCallback(
    CS_Source source, const char* name, enum CS_PropertyKind kind, int minimum,
    int maximum, int step, int defaultValue, int value, void* data,
    void (*onChange)(void* data, CS_Property property), CS_Status* status) {
  return cs::CreateSourcePropertyCallback(
      source, name, kind, minimum, maximum, step, defaultValue, value,
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
