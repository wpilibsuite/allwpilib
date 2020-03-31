/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ImageSourceImpl.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <wpi/STLExtras.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "c_util.h"
#include "cscore_cpp.h"
#include "cscore_raw.h"

using namespace cs;

ImageSourceImpl::ImageSourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                                 const VideoMode& mode)
    : SourceImpl{name, logger} {
  m_mode = mode;
  m_videoModes.push_back(m_mode);
}

ImageSourceImpl::~ImageSourceImpl() {}

void ImageSourceImpl::Start() {
  connected();
  videoModesUpdated();
  videoModeChanged(m_mode);
}

bool ImageSourceImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  {
    std::scoped_lock lock(m_mutex);
    m_mode = mode;
    m_videoModes[0] = mode;
  }
  videoModeChanged(mode);
  return true;
}

void ImageSourceImpl::NumSinksChanged() {
  // ignore
}

void ImageSourceImpl::NumSinksEnabledChanged() {
  // ignore
}

void ImageSourceImpl::NotifyError(const wpi::Twine& msg) {
  PutError(msg, wpi::Now());
}

int ImageSourceImpl::CreateProperty(const wpi::Twine& name,
                                    CS_PropertyKind kind, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value) {
  std::scoped_lock lock(m_mutex);
  auto [ndx, prop] = CreateOrUpdateProperty(
      name,
      [=] {
        return std::make_unique<PropertyImpl>(name, kind, minimum, maximum,
                                              step, defaultValue, value);
      },
      [&](auto& prop) {
        // update all but value
        prop->propKind = kind;
        prop->minimum = minimum;
        prop->maximum = maximum;
        prop->step = step;
        prop->defaultValue = defaultValue;
        value = prop->value;
      });
  propertyCreated(ndx, *prop);
  return ndx;
}

void ImageSourceImpl::SetEnumPropertyChoices(int property,
                                             wpi::ArrayRef<std::string> choices,
                                             CS_Status* status) {
  std::scoped_lock lock(m_mutex);
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
  propertyChoicesUpdated(property, *prop);
}

void ImageSourceImpl::PutFrame(cv::Mat& image) {
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

void ImageSourceImpl::PutFrame(const CS_RawFrame& image) {
  int type;
  switch (image.pixelFormat) {
    case VideoMode::kYUYV:
    case VideoMode::kRGB565:
      type = CV_8UC2;
      break;
    case VideoMode::kBGR:
      type = CV_8UC3;
      break;
    case VideoMode::kGray:
    case VideoMode::kMJPEG:
    default:
      type = CV_8UC1;
      break;
  }
  cv::Mat finalImage{image.height, image.width, type, image.data};
  std::unique_ptr<Image> dest =
      AllocImage(static_cast<VideoMode::PixelFormat>(image.pixelFormat),
                 image.width, image.height, image.totalData);
  finalImage.copyTo(dest->AsMat());

  SourceImpl::PutFrame(std::move(dest), wpi::Now());
}

namespace cs {

CS_Source CreateImageSource(const wpi::Twine& name, const VideoMode& mode,
                            CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_IMAGE, std::make_shared<ImageSourceImpl>(
                                                name, inst.GetLogger(), mode));
}

CS_Source CreateCvSource(const wpi::Twine& name, const VideoMode& mode,
                         CS_Status* status) {
  return CreateImageSource(name, mode, status);
}

CS_Source CreateRawSource(const wpi::Twine& name, const VideoMode& mode,
                          CS_Status* status) {
  return CreateImageSource(name, mode, status);
}

void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_IMAGE) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSourceImpl&>(*data->source).PutFrame(image);
}

void PutSourceFrame(CS_Source source, const CS_RawFrame& image,
                    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_IMAGE) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSourceImpl&>(*data->source).PutFrame(image);
}

void NotifySourceError(CS_Source source, const wpi::Twine& msg,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & CS_SOURCE_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & CS_SOURCE_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, const wpi::Twine& description,
                          CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & CS_SOURCE_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSourceImpl&>(*data->source).SetDescription(description);
}

CS_Property CreateSourceProperty(CS_Source source, const wpi::Twine& name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & CS_SOURCE_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<ImageSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value);
  return Handle{source, property, Handle::kProperty};
}

void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  wpi::ArrayRef<std::string> choices,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & CS_SOURCE_IMAGE) == 0) {
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
  auto data2 = Instance::GetInstance().GetSource(Handle{i, Handle::kSource});
  if (!data2 || data->source.get() != data2->source.get()) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  int propertyIndex = handle.GetIndex();
  static_cast<ImageSourceImpl&>(*data->source)
      .SetEnumPropertyChoices(propertyIndex, choices, status);
}

}  // namespace cs

extern "C" {

CS_Source CS_CreateImageSource(const char* name, const CS_VideoMode* mode,
                               CS_Status* status) {
  return cs::CreateImageSource(name, static_cast<const cs::VideoMode&>(*mode),
                               status);
}

CS_Source CS_CreateCvSource(const char* name, const CS_VideoMode* mode,
                            CS_Status* status) {
  return cs::CreateImageSource(name, static_cast<const cs::VideoMode&>(*mode),
                               status);
}

CS_Source CS_CreateRawSource(const char* name, const CS_VideoMode* mode,
                             CS_Status* status) {
  return cs::CreateImageSource(name, static_cast<const cs::VideoMode&>(*mode),
                               status);
}

#if CV_VERSION_MAJOR < 4
void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::PutSourceFrame(source, mat, status);
}
#endif  // CV_VERSION_MAJOR < 4

void CS_PutSourceFrameCpp(CS_Source source, cv::Mat* image, CS_Status* status) {
  return cs::PutSourceFrame(source, *image, status);
}

void CS_PutRawSourceFrame(CS_Source source, const struct CS_RawFrame* image,
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

void CS_SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                     const char** choices, int count,
                                     CS_Status* status) {
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) vec.push_back(choices[i]);
  return cs::SetSourceEnumPropertyChoices(source, property, vec, status);
}

}  // extern "C"
