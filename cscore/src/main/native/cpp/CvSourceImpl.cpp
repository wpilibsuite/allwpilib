// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CvSourceImpl.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <wpi/STLExtras.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "Notifier.h"
#include "c_util.h"
#include "cscore_cpp.h"

using namespace cs;

CvSourceImpl::CvSourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                           Notifier& notifier, Telemetry& telemetry,
                           const VideoMode& mode)
    : ConfigurableSourceImpl{name, logger, notifier, telemetry, mode} {}

CvSourceImpl::~CvSourceImpl() = default;

void CvSourceImpl::PutFrame(cv::Mat& image) {
  // We only support 8-bit images; convert if necessary.
  cv::Mat finalImage;
  if (image.depth() == CV_8U) {
    finalImage = image;
  } else {
    image.convertTo(finalImage, CV_8U);
  }

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

namespace cs {

CS_Source CreateCvSource(const wpi::Twine& name, const VideoMode& mode,
                         CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_CV, std::make_shared<CvSourceImpl>(
                                             name, inst.logger, inst.notifier,
                                             inst.telemetry, mode));
}

void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).PutFrame(image);
}

static constexpr unsigned SourceMask = CS_SINK_CV | CS_SINK_RAW;

void NotifySourceError(CS_Source source, const wpi::Twine& msg,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, const wpi::Twine& description,
                          CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSourceImpl&>(*data->source).SetDescription(description);
}

CS_Property CreateSourceProperty(CS_Source source, const wpi::Twine& name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<CvSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value);
  return Handle{source, property, Handle::kProperty};
}

CS_Property CreateSourcePropertyCallback(
    CS_Source source, const wpi::Twine& name, CS_PropertyKind kind, int minimum,
    int maximum, int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<CvSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value, onChange);
  return Handle{source, property, Handle::kProperty};
}

void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  wpi::ArrayRef<std::string> choices,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
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
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec.push_back(choices[i]);
  }
  return cs::SetSourceEnumPropertyChoices(source, property, vec, status);
}

}  // extern "C"
